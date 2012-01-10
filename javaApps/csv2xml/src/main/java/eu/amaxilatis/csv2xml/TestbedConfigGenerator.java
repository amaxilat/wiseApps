package eu.amaxilatis.csv2xml;

import org.apache.log4j.Logger;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.StringTokenizer;

class TestbedConfigGenerator {
    private static final Logger log = Logger.getLogger(TestbedConfigGenerator.class);


    private final String HostsFilename;
    private final String portalFile;
    private final String testbedSetupFile;
    private final String wiseMLFilename;
    private final String XMLFilename;

    private final CsvTestbed csvTestbed;
    private String portalHostname;


    TestbedConfigGenerator(String testbed_name, CsvTestbed csvTestbed) {

        HostsFilename = "testbeds/" + testbed_name + "/hosts";
        portalFile = "testbeds/" + testbed_name + "/portal.csv";
        testbedSetupFile = "testbeds/" + testbed_name + "/testbed-setup.csv";
        wiseMLFilename = "testbeds/" + testbed_name + "/conf/tr.iwsn-wiseml.xml";
        XMLFilename = "testbeds/" + testbed_name + "/conf/tr.iwsn-testbed.xml";
        this.csvTestbed = csvTestbed;
    }

    void createWiseML() {

        StringBuilder WISEMLoutput = new StringBuilder();

        WISEMLoutput.append(""
                + "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                + "<wiseml version=\"1.0\" xmlns=\"http://wisebed.eu/ns/wiseml/1.0\">\n"
                + "\t<setup>\n");


        WISEMLoutput.append(TestbedDescription());


        //add nodes description
        for (String nodeId : csvTestbed.getNodes()) {
            final CsvNode currentCsvNode = csvTestbed.getNode(nodeId);
            WISEMLoutput.append(currentCsvNode.toDeviceWiseml());
        }

        WISEMLoutput.append(""
                + "\t</setup>\n"
                + "</wiseml>\n");

        save2file(wiseMLFilename, WISEMLoutput.toString());

    }

    private String TestbedDescription() {
        StringBuilder testbedDescriptionBuilder = new StringBuilder();

        try {
            //create BufferedReader to read csv file
            BufferedReader br = new BufferedReader(new FileReader(testbedSetupFile));
            String strLine;
            StringTokenizer st;

            //log.info("Reading nodes from csv file:");
            // read all lines of the file
            br.readLine();
            strLine = br.readLine();

            st = new StringTokenizer(strLine, ",");

            final String coordinateType = st.nextToken();
            final String coordinateX = st.nextToken();
            final String coordinateY = st.nextToken();
            final String coordinateZ = st.nextToken();
            final String coordinatePhi = st.nextToken();
            final String coordinateTheta = st.nextToken();
            final String description = st.nextToken();


            testbedDescriptionBuilder.append("\t\t<coordinateType>");
            testbedDescriptionBuilder.append(coordinateType);
            testbedDescriptionBuilder.append("</coordinateType>\n");
            testbedDescriptionBuilder.append("\t\t<origin>\n");
            testbedDescriptionBuilder.append("\t\t\t<x>");
            testbedDescriptionBuilder.append(coordinateX);
            testbedDescriptionBuilder.append("</x>\n");
            testbedDescriptionBuilder.append("\t\t\t<y>");
            testbedDescriptionBuilder.append(coordinateY);
            testbedDescriptionBuilder.append("</y>\n");
            testbedDescriptionBuilder.append("\t\t\t<z>");
            testbedDescriptionBuilder.append(coordinateZ);
            testbedDescriptionBuilder.append("</z>\n");
            testbedDescriptionBuilder.append("\t\t\t<phi>");
            testbedDescriptionBuilder.append(coordinatePhi);
            testbedDescriptionBuilder.append("</phi>\n");
            testbedDescriptionBuilder.append("\t\t\t<theta>");
            testbedDescriptionBuilder.append(coordinateTheta);
            testbedDescriptionBuilder.append("</theta>\n");
            testbedDescriptionBuilder.append("\t\t</origin>\n");
            testbedDescriptionBuilder.append("\t\t<description>");
            testbedDescriptionBuilder.append(description);
            testbedDescriptionBuilder.append("</description>\n");

        } catch (
                Exception e
                )

        {
            log.info(e.toString());
        }

        return testbedDescriptionBuilder.toString();
    }

    void createXML() {


        StringBuilder XMLoutput = new StringBuilder();


        portalHostname = "";

        XMLoutput.append(getTestbedPortal2XML());

        log.info("================Hosts Info =================");

        log.info("Already added devices for " + portalHostname);
        for (String hostname : csvTestbed.getHosts()) {
            final Host currentHost = csvTestbed.getHost(hostname);
            //do not add the portal serve
            if (currentHost.hostname().equals(portalHostname)) {
                continue;
            }
            //do not add xbee hosts
            if (currentHost.hostname().equals("null")) {
                continue;
            }
            log.info("hostname:" + currentHost.hostname());
            XMLoutput.append("\t<nodes id=\"" + currentHost.hostname() + "\">\n"
                    + "\t\t<names>\n");

            for (String nodeId : csvTestbed.getNodes()) {
                final CsvNode currentCsvNode = csvTestbed.getNode(nodeId);
                if (currentHost.hostname().equals(currentCsvNode.hostname())) {
                    XMLoutput.append(currentCsvNode.toNodeNameXml());
                }
            }

            log.info("\taddress:" + currentHost.host_addr());
            XMLoutput.append("\t\t</names>\n"
                    + "\t\t<serverconnections>\n"
                    + "\t\t\t<serverconnection address=\"" + currentHost.host_addr() + ":8880\" type=\"tcp\"/>\n"
                    + "\t\t</serverconnections>\n"
                    + "\t\t<applications>\n");
            XMLoutput.append("\t\t\t<application factoryclass = \"de.uniluebeck.itm.tr.runtime.wsndeviceobserver.WSNDeviceObserverFactory\" name = \"WSNDeviceObserver\" >\n");
            XMLoutput.append("\t\t\t\t<ns5:wsnDeviceObserver>\n");
            for (String nodeId : csvTestbed.getNodes()) {
                final CsvNode currentCsvNode = csvTestbed.getNode(nodeId);
                if (currentHost.hostname().equals(currentCsvNode.hostname())) {
                    XMLoutput.append("\t\t\t\t\t");
                    XMLoutput.append("<mapping mac=\"" + currentCsvNode.mac() + "\"");
                    if (currentCsvNode.type().equals("telosb")) {
                        XMLoutput.append(" usbchipid=\"" + currentCsvNode.usbchipid() + "\" ");
                    }
                    XMLoutput.append("/>\n");
                }
            }
            XMLoutput.append("\t\t\t\t</ns5:wsnDeviceObserver>\n");
            XMLoutput.append("\t\t\t</application>\n");

            for (String nodeId : csvTestbed.getNodes()) {
                final CsvNode currentCsvNode = csvTestbed.getNode(nodeId);
                if (currentHost.hostname().equals(currentCsvNode.hostname())) {
                    log.info("\t\tdevice:" + currentCsvNode.id() + "\ttype:"
                            + currentCsvNode.type());


                    XMLoutput.append(currentCsvNode.toDeviceXml());

                }
            }


            XMLoutput.append(""
                    + "\t\t</applications>\n");
            XMLoutput.append("\t</nodes>\n");


        }
        XMLoutput.append("</ns2:testbed>\n");

        save2file(XMLFilename, XMLoutput.toString());


    }

    private String getTestbedPortal2XML() {
        StringBuilder XMLoutput = new StringBuilder();
        try {

            // Input csv file

            //create BufferedReader to read csv file
            BufferedReader br = new BufferedReader(new FileReader(portalFile));
            String strLine;
            StringTokenizer st;

            log.info("Reading portal info from csv file:" + portalFile);
            // read all lines of the file
            strLine = br.readLine();
            st = new StringTokenizer(strLine, ",");
            portalHostname = st.nextToken();
            String name = st.nextToken();
            String address = st.nextToken();
            String urnprefix = st.nextToken();
            String sessionmanagementendpointurl = st.nextToken();
            String wsninstancebaseurl = st.nextToken();
            String reservationendpointurl = st.nextToken();
            String protobufport = st.nextToken();
            String protobufip = st.nextToken();
            log.info("================Portal Info ================");
            log.info("id:" + portalHostname);
            log.info("name:" + name);
            log.info("address:" + address);
            log.info("urnprefix:" + urnprefix);
            log.info("sessionmanagementendpointurl:" + sessionmanagementendpointurl);
            log.info("wsninstancebaseurl:" + wsninstancebaseurl);
            log.info("reservationendpointurl:" + reservationendpointurl);

            XMLoutput.append("<ns2:testbed xmlns:ns2=\"http://itm.uniluebeck.de/tr/xml\" "
                    + "xmlns:portal=\"http://itm.uniluebeck.de/tr/runtime/portalapp/xml\" "
                    + "xmlns:ns5=\"http://itm.uniluebeck.de/tr/runtime/wsndeviceobserver/config\" "
                    + "xmlns:wsn=\"http://itm.uniluebeck.de/tr/runtime/wsnapp/xml\">\n");

            XMLoutput.append(

                    "\t<nodes id=\"" + portalHostname + "\">\n"
                            + "\t\t<names>\n"
                            + "\t\t\t<nodename name=\"" + name + "\"/>\n");

            for (String nodeId : csvTestbed.getNodes()) {
                CsvNode currentCsvNode = csvTestbed.getNode(nodeId);
                if (currentCsvNode.hostname().equals(portalHostname)) {
                    XMLoutput.append(currentCsvNode.toNodeNameXml());
                }
            }

            XMLoutput.append(
                    "\t\t</names>\n"
                            + "\t\t<serverconnections>\n"
                            + "\t\t\t<serverconnection address=\"" + address + "\" type=\"tcp\"/>\n"
                            + "\t\t</serverconnections>\n"
                            + "\t\t<applications>\n"
                            + "\t\t\t<application  name=\"PortalServer\" factoryclass=\"de.uniluebeck.itm.tr.runtime.portalapp.PortalServerFactory\">\n"
                            + "\t\t\t\t<portal:portalapp>\n"
                            + "\t\t\t\t\t<webservice>\n"
                            + "\t\t\t\t\t\t<urnprefix>" + urnprefix + "</urnprefix>\n"
                            + "\t\t\t\t\t\t<sessionmanagementendpointurl>" + sessionmanagementendpointurl + "</sessionmanagementendpointurl>\n"
                            + "\t\t\t\t\t\t<wsninstancebaseurl>" + wsninstancebaseurl + "</wsninstancebaseurl>\n"
                            + "\t\t\t\t\t\t<reservationendpointurl>" + reservationendpointurl + "</reservationendpointurl>\n"
                            + "\t\t\t\t\t\t<wisemlfilename>../conf/tr.iwsn-wiseml.xml</wisemlfilename>\n"
                            + "\t\t\t\t\t\t<protobufinterface>\n"
                            + "\t\t\t\t\t\t\t<port>" + protobufport + "</port>\n"
                            + "\t\t\t\t\t\t<ip>" + protobufip + "</ip>\n"
                            + "\t\t\t\t\t</protobufinterface>\n"
                            + "\t\t\t\t\t</webservice>\n"
                            + "\t\t\t\t</portal:portalapp>\n"
                            + "\t\t\t</application>\n"
                            + "\t\t\t<application name=\"OverlaySocketConnector\" factoryclass=\"de.uniluebeck.itm.tr.runtime.socketconnector.server.SocketConnectorApplicationFactory\">\n"
                            + "\t\t\t\t<sc:socketconnector xmlns:sc=\"http://itm.uniluebeck.de/tr/runtime/socketconnector/server/xml\">\n"
                            + "\t\t\t\t\t<port>1234</port>\n"
                            + "\t\t\t\t</sc:socketconnector>\n"
                            + "\t\t\t</application>\n");
            for (String nodeId : csvTestbed.getNodes()) {
                CsvNode currentCsvNode = csvTestbed.getNode(nodeId);
                if (currentCsvNode.hostname().equals(portalHostname)) {
                    log.info("\t\tdevice:" + currentCsvNode.id() + "\ttype:"
                            + currentCsvNode.type());


                    XMLoutput.append(currentCsvNode.toDeviceXml());

                }
            }
            XMLoutput.append("\t\t</applications>\n"
                    + "\t</nodes>\n");
        } catch (Exception e) {
            log.info(e.toString());

        }
        return XMLoutput.toString();
    }

    public void createHosts() {


        StringBuilder HostsOutput = new StringBuilder();
        for (String hostname : csvTestbed.getHosts()) {
            Host currentHost = csvTestbed.getHost(hostname);
            if (currentHost.host_addr().equals("")) {
            } else if (currentHost.host_addr().equals("null")) {
            } else {
                HostsOutput.append(currentHost.host_addr());
                HostsOutput.append("\n");
            }
        }

        save2file(HostsFilename, HostsOutput.toString());
    }

    private void save2file(String filename, String output) {
        BufferedWriter out = null;
        try {
            out = new BufferedWriter(new FileWriter(filename));
            out.write(output);

            log.info(filename + " generated. Writing file: Ok!");
        } catch (Exception e) {
            log.error(e.toString());
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (IOException ignore) {

                }
            }
        }
    }
}


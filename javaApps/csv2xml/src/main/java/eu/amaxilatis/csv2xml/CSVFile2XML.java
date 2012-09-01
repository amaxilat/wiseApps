package eu.amaxilatis.csv2xml;

import com.sun.org.apache.xml.internal.serialize.OutputFormat;
import com.sun.org.apache.xml.internal.serialize.XMLSerializer;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

/**
 * Created with IntelliJ IDEA.
 * User: amaxilatis
 * Date: 9/1/12
 * Time: 12:46 PM
 * To change this template use File | Settings | File Templates.
 */
public class CSVFile2XML {
    private final Map<Host, List<Node>> hosts;
    private String testbedCsvFile;

    public CSVFile2XML(final String testbedCsvFile) {
        this.testbedCsvFile = testbedCsvFile;
        hosts = new HashMap<Host, List<Node>>();
        try {
            // Open the file that is the first
            // command line parameter
            FileInputStream fstream = new FileInputStream(testbedCsvFile);
            // Get the object of DataInputStream
            DataInputStream in = new DataInputStream(fstream);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            String strLine = br.readLine();

            //Read File Line By Line
            while ((strLine = br.readLine()) != null) {
                // Print the content on the console
                parseLine(strLine);
            }
            //Close the input stream
            in.close();
        } catch (Exception e) {//Catch exception if any
            e.printStackTrace();
            System.err.println("Error: " + e.getMessage());
        }

        for (Host host : hosts.keySet()) {
            List<Node> nodes = hosts.get(host);
            System.out.println("---------" + host);
            for (Node node : nodes) {
                System.out.println(node);
            }

        }

        try {
            generateXML();
        } catch (IOException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        } catch (SAXException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
    }

    private void generateXML() throws IOException, SAXException {
        FileOutputStream fos = new FileOutputStream("tr.iwsn-testbed.xml");
// XERCES 1 or 2 additionnal classes.
        OutputFormat of = new OutputFormat("XML", "UTF-8", true);
        of.setIndent(1);
        of.setIndenting(true);
        of.setStandalone(true);
        of.setOmitXMLDeclaration(true);


        XMLSerializer serializer = new XMLSerializer(fos, of);
// SAX2.0 ContentHandler.
        ContentHandler hd = serializer.asContentHandler();
        hd.startDocument();


// Processing instruction sample.
//hd.processingInstruction("xml-stylesheet","type=\"text/xsl\" href=\"users.xsl\"");
// USER attributes.
        AttributesImpl testbedAttrs = new AttributesImpl();

        testbedAttrs.addAttribute("", "", "xmlns:ns2", "cdata", "http://itm.uniluebeck.de/tr/xml");


// USERS tag.
        hd.startElement("", "", "ns2:testbed", testbedAttrs);

        generatePortalHost(hd);

        for (Host host : hosts.keySet()) {
            if (host.getHostname().equals("null")) continue;
            //	<nodes id="host">
            AttributesImpl atts = new AttributesImpl();
            atts.addAttribute("", "", "id", "cdata", host.getHostname());
            hd.startElement("", "", "nodes", atts);

            //<names>
            getNamesSection(host, hd);
            host.getServeConnections(hd);
            hd.startElement("", "", "applications", new AttributesImpl());
            getApplications(host, hd);
            hd.endElement("", "", "applications");

            hd.endElement("", "", "nodes");

        }
        hd.endElement("", "", "ns2:testbed");
        hd.endDocument();
        fos.close();
    }

    private void generatePortalHost(ContentHandler hd) throws SAXException {
        String filename = testbedCsvFile.substring(0, testbedCsvFile.lastIndexOf("/"));
        System.out.println(filename + "/portal.prop");
        Properties property = new Properties();
        try {
            property.load(new FileInputStream(filename + "/portal.prop"));

            addPortalApplication(hd, property);


            addOverlaySocketConnectorApplication(hd, property);

            hd.endElement("", "", "applications");


            hd.endElement("", "", "nodes");
        } catch (IOException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }


    }

    private void addPortalApplication(ContentHandler hd, Properties property) throws SAXException {
        AttributesImpl atts = new AttributesImpl();
        atts.addAttribute("", "", "id", "cdata", property.getProperty("host"));
        hd.startElement("", "", "nodes", atts);

        hd.startElement("", "", "names", new AttributesImpl());
        atts.clear();
        atts.addAttribute("", "", "name", "cdata", property.getProperty("address"));
        hd.startElement("", "", "nodename", atts);
        hd.endElement("", "", "nodename");
        hd.endElement("", "", "names");

        hd.startElement("", "", "serverconnections", new AttributesImpl());
        atts.clear();
        atts.addAttribute("", "", "address", "cdata", property.getProperty("address") + ":8880");
        atts.addAttribute("", "", "type", "cdata", "tcp");
        hd.startElement("", "", "serverconnection", atts);
        hd.endElement("", "", "serverconnection");
        hd.endElement("", "", "serverconnections");

        hd.startElement("", "", "applications", new AttributesImpl());
        atts.clear();
        atts.addAttribute("", "", "name", "cdata", "PortalServer");
        atts.addAttribute("", "", "factoryclass", "cdata", "de.uniluebeck.itm.tr.runtime.portalapp.PortalServerFactory");
        hd.startElement("", "", "application", atts);
        atts.clear();
        atts.addAttribute("", "", "xmlns:portal", "cdata", "http://itm.uniluebeck.de/tr/runtime/portalapp/xml");
        hd.startElement("", "", "portal:portalapp", atts);
        hd.startElement("", "", "webservice", new AttributesImpl());

        addElement(hd, "urnprefix", property);
        addElement(hd, "sessionmanagementendpointurl", property);
        addElement(hd, "wsninstancebaseurl", property);
        addElement(hd, "reservationendpointurl", property);
        addElement(hd, "snaaendpointurl", property);
        addElement(hd, "wisemlfilename", property);


        hd.startElement("", "", "protobufinterface", new AttributesImpl());
        hd.startElement("", "", "port", new AttributesImpl());
        addProperty(hd, "protobufinterface.port", property);
        hd.endElement("", "", "port");
        hd.startElement("", "", "ip", new AttributesImpl());
        addProperty(hd, "protobufinterface.ip", property);
        hd.endElement("", "", "ip");
        hd.endElement("", "", "protobufinterface");


        hd.endElement("", "", "webservice");
        hd.endElement("", "", "portal:portalapp");
        hd.endElement("", "", "application");
    }

    private void addOverlaySocketConnectorApplication(ContentHandler hd, Properties property) throws SAXException {
        AttributesImpl atts = new AttributesImpl();
        atts.clear();
        atts.addAttribute("", "", "name", "cdata", "OverlaySocketConnector");
        atts.addAttribute("", "", "factoryclass", "cdata", "de.uniluebeck.itm.tr.runtime.socketconnector.server.SocketConnectorApplicationFactory");
        hd.startElement("", "", "application", atts);
        atts.clear();
        atts.addAttribute("", "", "xmlns:sc", "cdata", "http://itm.uniluebeck.de/tr/runtime/socketconnector/server/xml");
        hd.startElement("", "", "sc:socketconnector", atts);
        hd.startElement("", "", "port", new AttributesImpl());
        addProperty(hd, "socketconnector.port", property);
        hd.endElement("", "", "port");
        hd.endElement("", "", "sc:socketconnector");

        hd.endElement("", "", "application");

    }

    private void addElement(ContentHandler hd, String elem, Properties property) throws SAXException {
        hd.startElement("", "", elem, new AttributesImpl());
        addProperty(hd, elem, property);
        hd.endElement("", "", elem);

    }

    private void addProperty(ContentHandler hd, String urnprefix, Properties property) throws SAXException {
        char[] value = property.getProperty(urnprefix).toCharArray();
        hd.characters(value, 0, value.length);
    }

    private void getApplications(Host host, ContentHandler hd) throws SAXException {
        AttributesImpl atts = new AttributesImpl();
        atts.addAttribute("", "", "factoryclass", "cdata", "de.uniluebeck.itm.tr.runtime.wsndeviceobserver.WSNDeviceObserverFactory");
        atts.addAttribute("", "", "name", "cdata", "WSNDeviceObserver");
        hd.startElement("", "", "application", atts);
        atts.clear();
        atts.addAttribute("", "", "xmlns:do", "cdata", "http://itm.uniluebeck.de/tr/runtime/wsndeviceobserver/config");
        hd.startElement("", "", "do:wsnDeviceObserver", atts);
        for (Node node : hosts.get(host)) {
            if (node.getType().contains("motap")) continue;
            atts.clear();
            atts.addAttribute("", "", "mac", "cdata", node.getMac());
            if (node.getType().equals("telosb")) {
                atts.addAttribute("", "", "usbchipid", "cdata", node.getChipid());
            }
            hd.startElement("", "", "mapping", atts);
            hd.endElement("", "", "mapping");
        }
        hd.endElement("", "", "do:wsnDeviceObserver");
        hd.endElement("", "", "application");

        for (Node node : hosts.get(host)) {
            if (node.getType().contains("motap")) continue;
            node.getNodeApplication(hd);
        }


    }


    private void getNamesSection(Host host, ContentHandler hd) throws SAXException {
        AttributesImpl atts = new AttributesImpl();
        hd.startElement("", "", "names", atts);
        for (Node node : hosts.get(host)) {
            node.getNodename(hd);
        }
        hd.endElement("", "", "names");
    }


    private void parseLine(String strLine) {
        String[] parts = strLine.split(",");
        Host newHost = new Host(strLine);
        if (!hosts.containsKey(newHost)) {

            hosts.put(newHost, new ArrayList<Node>());
        }
        List<Node> nodes = hosts.get(newHost);
        Node newNode = new Node(parts);
        nodes.add(newNode);
    }
}

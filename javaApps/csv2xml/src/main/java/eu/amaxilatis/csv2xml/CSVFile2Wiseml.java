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
public class CSVFile2Wiseml {
    private final Map<Host, List<Node>> hosts;
    private String testbedCsvFile;

    public CSVFile2Wiseml(final String testbedCsvFile) {
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

        int hostCounter = 0;
        int nodeCounter = 0;

        for (Host host : hosts.keySet()) {
            hostCounter++;
            List<Node> nodes = hosts.get(host);
            System.out.println("---------" + host);
            for (Node node : nodes) {
                System.out.println(node);
                nodeCounter++;
            }
        }
        System.out.println("Hosts: "+hostCounter);
        System.out.println("Nodes: "+nodeCounter);

        try {
            generateWiseml();
        } catch (IOException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        } catch (SAXException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
    }

    private void generateWiseml() throws IOException, SAXException {
        FileOutputStream fos = new FileOutputStream("tr.iwsn-wiseml.xml");
// XERCES 1 or 2 additionnal classes.
        OutputFormat of = new OutputFormat("XML", "UTF-8", true);
        of.setIndent(1);
        of.setIndenting(true);
        of.setOmitDocumentType(true);
        of.setStandalone(true);


        XMLSerializer serializer = new XMLSerializer(fos, of);
// SAX2.0 ContentHandler.
        ContentHandler hd = serializer.asContentHandler();
        hd.startDocument();

        String filename = testbedCsvFile.substring(0, testbedCsvFile.lastIndexOf("/"));
        System.out.println(filename + "/portal.prop");
        Properties property = new Properties();
        try {
            property.load(new FileInputStream(filename + "/portal.prop"));
        } catch (IOException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }


// Processing instruction sample.
//hd.processingInstruction("xml-stylesheet","type=\"text/xsl\" href=\"users.xsl\"");
// USER attributes.
        AttributesImpl testbedAttrs = new AttributesImpl();
        testbedAttrs.addAttribute("", "", "version", "cdata", "1.0");
        testbedAttrs.addAttribute("", "", "xmlns", "cdata", "http://wisebed.eu/ns/wiseml/1.0");

// USERS tag.
        hd.startElement("", "", "wiseml", testbedAttrs);
        testbedAttrs.clear();
        hd.startElement("", "", "setup", testbedAttrs);

        addElement(hd, "coordinateType", property);

        hd.startElement("", "", "origin", testbedAttrs);
        addElement(hd, "x", property);
        addElement(hd, "y", property);
        addElement(hd, "z", property);
        addElement(hd, "phi", property);
        addElement(hd, "theta", property);
        hd.endElement("", "", "origin");

        addElement(hd, "description", property);

        for (Host host : hosts.keySet()) {
            for (Node node : hosts.get(host)) {
                node.getWiseml(hd);
            }
        }

        hd.endElement("", "", "setup");
        hd.endElement("", "", "wiseml");
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
        atts.addAttribute("", "", "address", "cdata", property.getProperty("address") + ":8080");
        atts.addAttribute("", "", "type", "cdata", "tcp");
        hd.startElement("", "", "serverconnection", atts);
        hd.endElement("", "", "serverconnection");
        hd.endElement("", "", "serverconnections");

        hd.startElement("", "", "applications", new AttributesImpl());
        atts.clear();
        atts.addAttribute("", "", "name", "cdata", "PortalServer");
        atts.addAttribute("", "", "factoryclass", "cdata", "de.uniluebeck.itm.tr.runtime.portalapp.PortalServerFactory");
        hd.startElement("", "", "application", atts);
        hd.startElement("", "", "portal:portalapp", new AttributesImpl());
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
        hd.startElement("", "", "ns5:wsnDeviceObserver", new AttributesImpl());
        for (Node node : hosts.get(host)) {
            atts.clear();
            atts.addAttribute("", "", "mac", "cdata", node.getMac());
            hd.startElement("", "", "mapping", atts);
            hd.endElement("", "", "mapping");
        }
        hd.endElement("", "", "ns5:wsnDeviceObserver");
        hd.endElement("", "", "application");

        for (Node node : hosts.get(host)) {
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

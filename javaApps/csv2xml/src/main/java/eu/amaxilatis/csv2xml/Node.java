package eu.amaxilatis.csv2xml;

import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

/**
 * Created with IntelliJ IDEA.
 * User: amaxilatis
 * Date: 9/1/12
 * Time: 1:12 PM
 * To change this template use File | Settings | File Templates.
 */
public class Node {
    private String prefix;
    private String mac;
    private String port;
    private String type;
    private String chipid;
    private String room;
    private String posx;
    private String posy;
    private String posz;
    private String capabilities;
    private String datatypes;
    private String units;
    private String defaults;

    public String getPrefix() {
        return prefix;
    }

    public String getMac() {
        return mac;
    }

    public String getPort() {
        return port;
    }

    public String getType() {
        return type;
    }

    public String getChipid() {
        return chipid;
    }

    public String getRoom() {
        return room;
    }

    public String getPosx() {
        return posx;
    }

    public String getPosy() {
        return posy;
    }

    public String getPosz() {
        return posz;
    }

    public String getGateway() {
        return gateway;
    }

    private String gateway;


    public Node(String[] parts) {

        prefix = parts[2];
        mac = parts[3];
        port = parts[4];
        type = parts[5];
        chipid = parts[6];
        room = parts[7];
        posx = parts[8];
        posy = parts[9];
        posz = parts[10];
        gateway = parts[11];
        capabilities = parts[13];
        datatypes = parts[14];
        units = parts[15];
        defaults = parts[16];
    }


    @Override
    public String toString() {
        return "Node{" +
                "mac='" + mac + '\'' +
                '}';
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Node node = (Node) o;

        if (mac != null ? !mac.equals(node.mac) : node.mac != null) return false;

        return true;
    }

    @Override
    public int hashCode() {
        return mac != null ? mac.hashCode() : 0;
    }

    public void getNodename(ContentHandler hd) throws SAXException {
        AttributesImpl atts = new AttributesImpl();
        atts.addAttribute("", "", "name", "cdata", getPrefix() + getMac());
        hd.startElement("", "", "nodename", atts);
        hd.endElement("", "", "nodename");
    }

    public void getNodeApplication(ContentHandler hd) throws SAXException {
        AttributesImpl atts = new AttributesImpl();

        atts.addAttribute("", "", "name", "cdata", "WSNDeviceApp-" + getPrefix() + getMac());
        atts.addAttribute("", "", "factoryclass ", "cdata", "de.uniluebeck.itm.tr.runtime.wsnapp.WSNDeviceAppFactory");
        hd.startElement("", "", "application", atts);
        atts.clear();
        atts.addAttribute("", "", "xmlns:wsn", "cdata", "http://itm.uniluebeck.de/tr/runtime/wsnapp/xml");
        hd.startElement("", "", "wsn:wsnDevice", atts);

        hd.startElement("", "", "urn", new AttributesImpl());
        char[] fullAddress = (getPrefix() + getMac()).toCharArray();
        hd.characters(fullAddress, 0, fullAddress.length);
        hd.endElement("", "", "urn");

        hd.startElement("", "", "type", new AttributesImpl());
        char[] type = (getType()).toCharArray();
        hd.characters(type, 0, type.length);
        hd.endElement("", "", "type");

        hd.startElement("", "", "defaultImage", new AttributesImpl());
        char[] dimage = (getDefaultImage()).toCharArray();
        hd.characters(dimage, 0, dimage.length);
        hd.endElement("", "", "defaultImage");

        if (getType().equals("telosb")) {
            hd.startElement("", "", "usbchipid", new AttributesImpl());
            char[] chipid = (getChipid()).toCharArray();
            hd.characters(chipid, 0, chipid.length);
            hd.endElement("", "", "usbchipid");

        }

        addChannelPipeline(hd);


        hd.endElement("", "", "wsn:wsnDevice");
        hd.endElement("", "", "application");
    }

    private void addChannelPipeline(ContentHandler hd) throws SAXException {
        hd.startElement("", "", "defaultChannelPipeline", new AttributesImpl());
        hd.startElement("", "", "configurationXml", new AttributesImpl());
        hd.startElement("", "", "itm-netty-handlerstack", new AttributesImpl());
        AttributesImpl atts = new AttributesImpl();
        atts.addAttribute("", "", "name", "cdata", "dlestxetx-framing");
        atts.addAttribute("", "", "factory", "cdata", "dlestxetx-framing");
        hd.startElement("", "", "handler", atts);
        hd.endElement("", "", "handler");
        hd.endElement("", "", "itm-netty-handlerstack");
        hd.endElement("", "", "configurationXml");
        hd.endElement("", "", "defaultChannelPipeline");
    }

    public void getWiseml(ContentHandler hd) throws SAXException {
        AttributesImpl atts = new AttributesImpl();
        atts.addAttribute("", "", "id", "cdata", getPrefix() + getMac());
        hd.startElement("", "", "node", atts);

        hd.startElement("", "", "position", new AttributesImpl());
        addElement(hd, "x", getPosx());
        addElement(hd, "y", getPosy());
        addElement(hd, "z", getPosz());
        hd.endElement("", "", "position");
        addElement(hd, "gateway", getGateway());
        addElement(hd, "nodeType", getType());

        String[] caps = capabilities.split(" ");
        for (int i = 0; i < caps.length; i++) {
            hd.startElement("", "", "capability", new AttributesImpl());
            addElement(hd, "name", caps[i]);
            addElement(hd, "datatype", datatypes.split(" ")[i]);
            addElement(hd, "unit", units.split(" ")[i]);
            addElement(hd, "default", defaults.split(" ")[i]);

            hd.endElement("", "", "capability");
            addElement(hd, "description", "ROOM: " + room);
        }

        hd.endElement("", "", "node");
    }

    private void addElement(ContentHandler hd, String elem, String value) throws SAXException {
        hd.startElement("", "", elem, new AttributesImpl());
        char[] charvalue = value.toCharArray();
        hd.characters(charvalue, 0, charvalue.length);
        hd.endElement("", "", elem);
    }


    public String getDefaultImage() {
        if (getType().contains("isense")) {
            return "/home/testbedruntime/collector." + getType() + ".bin";
        } else {
            return "/home/testbedruntime/collector." + getType() + ".ihex";
        }
    }
}


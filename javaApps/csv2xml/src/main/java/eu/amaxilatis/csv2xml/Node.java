/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package eu.amaxilatis.csv2xml;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * @author amaxilatis
 */
class Node {

    Node(String hostname, String host_addr, String prefix,
         String node_id, String node_port, String node_type,
         String node_reference, String room,
         String x, String y, String z,
         String gateway, String semas) {

        semantics = new HashMap<String, String>();

        hostname_ = hostname;
        host_addr_ = host_addr;
        prefix_ = prefix;
        node_id_ = node_id;

        node_port_ = node_port;


        node_type_ = node_type;


        node_reference_ = node_reference;


        pos_x_ = java.lang.Double.parseDouble(x);
        pos_y_ = java.lang.Double.parseDouble(y);
        pos_z_ = java.lang.Double.parseDouble(z);
        room_ = room;
        gateway_ = gateway.toLowerCase();

        capabilities_ = new ArrayList<Capability>();

        for (String newsemantic : semas.split(" ")) {
            final String semantic_name = newsemantic.substring(0, newsemantic.indexOf(":"));
            final String semantic_value = newsemantic.substring(newsemantic.indexOf(":") + 1);
            semantics.put(semantic_name, semantic_value);
        }

    }

    @Override
    public String toString() {

        return " hostname:" + hostname_
                + " hostaddr:" + host_addr_
                + " prefix:" + prefix_
                + " node_id:" + node_id_
                + " node_type:" + node_type_
                + " node_port:" + node_port_
                + " node_reference:" + node_reference_;
    }

    public String toNodeNameXml() {

        return "\t\t\t<nodename name=\"" + prefix_ + node_id_ + "\"/>\n";

    }

    public String toDeviceXml() {
        StringBuilder deviceXml = new StringBuilder();
        deviceXml.append("\t\t\t<application factoryclass=\"de.uniluebeck.itm.tr.runtime.wsnapp.WSNDeviceAppFactory\" name=\"WSNDeviceApp\">\n"
                + "\t\t\t\t<ns4:wsnDevice>\n"
                + "\t\t\t\t\t<urn>" + prefix_ + node_id_ + "</urn>\n"
                + "\t\t\t\t\t<type>" + node_type_ + "</type>\n");
        if (!node_port_.equals("auto")) {
            deviceXml.append(
                    "\t\t\t\t\t<serialinterface>" + node_port_ + "</serialinterface>\n");
        }

        if (!node_type_.equals("isense")) {

            deviceXml.append("\t\t\t\t\t<usbchipid>" + node_reference_ + "</usbchipid>\n");
        }
        deviceXml.append("\t\t\t\t</ns4:wsnDevice>\n"
                + "\t\t\t</application>\n");

        return deviceXml.toString();
    }

    public String id() {
        return prefix_ + node_id_;
    }

    public String type() {
        return node_type_;
    }

    public String toDeviceWiseml() {
        StringBuilder deviceWiseml = new StringBuilder();
        deviceWiseml.append("\t\t<node id=\"" + prefix_ + node_id_ + "\">\n");
        deviceWiseml.append("\t\t\t<position>\n");
        deviceWiseml.append("\t\t\t\t<x>" + pos_x_ + "</x>\n");
        deviceWiseml.append("\t\t\t\t<y>" + pos_y_ + "</y>\n");
        deviceWiseml.append("\t\t\t\t<z>" + pos_z_ + "</z>\n");
        deviceWiseml.append("\t\t\t</position>\n");
        deviceWiseml.append("\t\t\t<gateway>" + gateway_ + "</gateway>\n");
        deviceWiseml.append("\t\t\t<nodeType>" + node_type_ + "</nodeType>\n");
        deviceWiseml.append(description());
        deviceWiseml.append("\t\t</node>\n");
        return deviceWiseml.toString();
    }

    public String hostname() {
        return hostname_;
    }


    public void add_capability(String tname, String tdatatype, String tunit, String tdefault) {
        //System.out.println("Node " + node_id_ + " capability " + tname);
        capabilities_.add(new Capability(tname, tdatatype, tunit, tdefault));

    }

    public String description() {
        StringBuilder output = new StringBuilder();


        for (Capability aCapabilities_ : capabilities_) {
            output.append(aCapabilities_.getDescription());
        }
        output.append(getNodeTypeDescription());
        return output.toString();

    }

    String getNodeTypeDescription() {

        StringBuilder val = new StringBuilder();
        val.append("\t\t\t<description>");
        val.append("Room:" + room_ + ", ");
        for (String key : semantics.keySet()) {
            val.append(key + ":" + semantics.get(key) + ", ");
        }
        val.append(getCpuInfo());
        val.append("</description>\n");
        return val.toString();
    }

    String getCpuInfo() {

        if (node_type_.equals("isense")) {
            return "Processor: Jennic JN5139R1 (Ram 96kB, Flash 128kB, op/sleep 39mA,10myA) Radio: IEEE 802.15.4(2,4 GHz)";
        } else if (node_type_.equals("telosb")) {
            return "Processor: MSP 430F1611 (Ram 10kB, Flash 48kB, op/sleep 22mA, 5myA) Radio: TI CC2420 IEEE 802.15.4(2,4 GHz)";
        } else if (node_type_.equals("isense-motap")) {
            return "Processor: Jennic JN5139R1 (Ram 96kB, Flash 128kB, op/sleep 39mA,10myA) Radio: IEEE 802.15.4(2,4 GHz)";
        } else if (node_type_.equals("xbee")) {
            return "Processor: Arduino Pro Mini ATmega168  Radio: XBee 1mW Series 1";
        }
        return "";
    }


    String getDescrioption() {
        StringBuilder val = new StringBuilder();
        val.append("Room:" + room_ + ", ");
        for (String key : semantics.keySet()) {
            val.append(key + ":" + semantics.get(key) + ", ");
        }
        val.append(getCpuInfo());
        return val.toString();
    }

    private String hostname_;
    private String host_addr_;
    private String prefix_;
    private String node_id_;
    private String node_port_;
    private String node_type_;
    private String node_reference_;
    private double pos_x_, pos_y_, pos_z_;
    private String gateway_;
    private String room_;
    private HashMap<String, String> semantics;

    private ArrayList<Capability> capabilities_;

    public float getX() {
        return (float) pos_x_;
    }

    public float getY() {
        return (float) pos_y_;
    }

    public float getZ() {
        return (float) pos_z_;
    }
}

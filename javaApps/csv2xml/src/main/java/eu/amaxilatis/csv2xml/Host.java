/*
* To change this template, choose Tools | Templates
* and open the template in the editor.
*/
package eu.amaxilatis.csv2xml;

import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

/**
 * @author amaxilatis
 */
class Host {

    private String hostname;
    private String hostAddr;

    Host(String str) {
        this.hostname = str.split(",")[0];
        this.hostAddr = str.split(",")[1];
    }

    public String getHostname() {
        return hostname;
    }

    public String getHostAddr() {
        return hostAddr;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Host host = (Host) o;

        if (hostname != null ? !hostname.equals(host.hostname) : host.hostname != null) return false;

        return true;
    }

    @Override
    public int hashCode() {
        return hostname != null ? hostname.hashCode() : 0;
    }

    @Override
    public String toString() {
        return "Host{" +
                "hostname='" + hostname + '\'' +
                '}';
    }

    public void getServeConnections(ContentHandler hd) throws SAXException {

        hd.startElement("", "", "serverconnections", new AttributesImpl());
        AttributesImpl atts1 = new AttributesImpl();
        atts1.addAttribute("", "", "address", "cdata", hostAddr + ":8880");
        atts1.addAttribute("", "", "type", "cdata", "tcp");
        hd.startElement("", "", "serverconnection", atts1);
        hd.endElement("", "", "serverconnection");
        hd.endElement("", "", "serverconnections");
    }
}

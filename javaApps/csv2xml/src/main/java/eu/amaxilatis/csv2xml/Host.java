/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package eu.amaxilatis.csv2xml;

/**
 * @author amaxilatis
 */
class Host {

    Host(String hostname, String host_addr) {
        hostname_ = hostname;
        host_addr_ = host_addr;
    }

    public String hostname() {
        return hostname_;
    }

    public String host_addr() {
        return host_addr_;
    }

    private String hostname_;
    private String host_addr_;
}

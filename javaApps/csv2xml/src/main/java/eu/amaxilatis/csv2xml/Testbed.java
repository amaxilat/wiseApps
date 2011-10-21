package eu.amaxilatis.csv2xml;

import org.apache.log4j.Logger;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashMap;
import java.util.StringTokenizer;

public class Testbed {
    private static final Logger log = Logger.getLogger(Testbed.class);

    private final HashMap<String, Node> nodes;


    public String[] getHosts() {
        String[] host = new String[HostMap.size()];
        int i = 0;
        for (String a : HostMap.keySet()) {
            host[i++] = a;
        }
        return host;
    }

    public String[] getNodes() {
        String[] host = new String[nodes.size()];
        int i = 0;
        for (String a : nodes.keySet()) {
            host[i++] = a;
        }
        return host;

    }

    public Host getHost(String hostname) {
        return HostMap.get(hostname);
    }

    private final HashMap<String, Host> HostMap;
    private final String inFile;
    private final String testbedSetupFile;


    public Testbed(String inFile, String testbedSetupFile) {
        nodes = new HashMap<String, Node>();
        HostMap = new HashMap<String, Host>();

        this.inFile = inFile;
        this.testbedSetupFile = testbedSetupFile;
        parse();
    }

    private void parse() {
        try {

            // Input csv file

            //create BufferedReader to read csv file
            BufferedReader br = new BufferedReader(new FileReader(inFile));
            String strLine;

            log.info("Reading info from csv files (" + inFile + "," + testbedSetupFile + ")...");
            br.readLine();
            // read all lines of the file
            while ((strLine = br.readLine()) != null) {

                parseLine(strLine);

            }

            log.info("Found " + HostMap.size() + " hosts.");
            log.info("Found " + nodes.size() + " nodes.");
        } catch (Exception e) {
            log.info(e.toString());
            e.printStackTrace();
        }

    }

    private void parseLine(String strLine) {
        //tokenize line using ,
        final StringTokenizer st = new StringTokenizer(strLine, ",");


        final String hostname = st.nextToken();
        //log.info(hostname);
        final String hostAddress = st.nextToken();
        //log.info(hostAddress);

        if (!HostMap.containsKey(hostname)) {
            HostMap.put(hostname, new Host(hostname, hostAddress));

        }

        final String prefix = st.nextToken();
//                log.info(prefix);
        final String node_id = st.nextToken();
//                log.info(node_id);
        final String node_port = st.nextToken();
//                log.info(node_port);
        final String node_type = st.nextToken();
//                log.info(node_type);
        final String node_reference = st.nextToken();
//                log.info(node_reference);
        final String room = st.nextToken();
//                log.info(room);
        final String x = st.nextToken();
//                log.info(x);
        final String y = st.nextToken();
//                log.info(y);
        final String z = st.nextToken();
//                log.info(z);
        final String gateway = st.nextToken();
//                log.info(gateway);
        final String semantics = st.nextToken();
//                log.info(semantics);

        Node newNode;
        if (!nodes.containsKey(node_id)) {
            newNode = new Node(hostname, hostAddress, prefix, node_id, node_port, node_type, node_reference,
                    room, x, y, z, gateway, semantics);
            if (st.hasMoreElements()) {
                final String[] capabilityName = st.nextToken().split(" ");
                final String[] capabilityDataType = st.nextToken().split(" ");
                final String[] capabilityUnit = st.nextToken().split(" ");
                final String[] capabilityDefaultValue = st.nextToken().split(" ");

                for (int i = 0; i < capabilityName.length; i++) {
                    newNode.add_capability(capabilityName[i], capabilityDataType[i], capabilityUnit[i], capabilityDefaultValue[i]);
                }
            }
            nodes.put(node_id, newNode);
        }
    }

    public Node getNode(String nodeId) {
        return nodes.get(nodeId);
    }
}

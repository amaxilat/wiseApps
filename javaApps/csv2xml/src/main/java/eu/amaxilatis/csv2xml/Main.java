package eu.amaxilatis.csv2xml;

import org.apache.log4j.Logger;


/**
 * @author amaxilatis
 */
public class Main {

    private static final Logger log = Logger.getLogger(Main.class);

    private static Testbed testbed;


    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {


        if (args.length < 1) {
            log.error("Error needs The name of the testbed as Argument!");
            return;
        }

        String testbed_name = args[0];
        if (args.length == 1) {
            createConfiguration(testbed_name);
        } else if (args.length == 2) {
            if (args[1].equals("config")) {
                createConfiguration(testbed_name);
                log.info("Application finished");
            } else if (args[1].equals("dbupdate")) {
                updateWiseDB(testbed_name);
                log.info("Application finished");
            } else {
                log.error("Please Specify Action {config,dbupdate}");
            }

        }


    }


    private static void updateWiseDB(String testbed_name) {
        init(testbed_name);
        WisedbUpdater wisedbUpdater = new WisedbUpdater(testbed_name, testbed);
        log.info("Updating wiseDB for Testbed: " + testbed_name);
    }

    private static void createConfiguration(String testbed_name) {

        init(testbed_name);
        log.info("Creating xml files for Testbed: " + testbed_name);
        TestbedConfigGenerator testbedConfigGenerator = new TestbedConfigGenerator(testbed_name, testbed);

        //BUILDING TESTBED.xml

        testbedConfigGenerator.createXML();

        //BUILDING WISEML.xml
        testbedConfigGenerator.createWiseML();

        //BUILDING hosts
        testbedConfigGenerator.createHosts();
    }

    private static void init(String testbed_name) {

        String inFile = "testbeds/" + testbed_name + "/nodes.csv";
        String testbedSetupFile = "testbeds/" + testbed_name + "/testbed-setup.csv";
        testbed = new Testbed(inFile, testbedSetupFile);
    }
}

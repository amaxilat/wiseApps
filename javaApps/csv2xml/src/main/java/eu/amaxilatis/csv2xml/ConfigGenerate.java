package eu.amaxilatis.csv2xml;

/**
 * @author amaxilatis
 */
public class ConfigGenerate {


    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {


        if (args.length < 1) {
            System.err.println("Error needs The name of the csvTestbed as Argument!");
            return;
        }

        String testbedCsvFile = args[0];
        System.out.println(testbedCsvFile);

        CSVFile2XML csv = new CSVFile2XML(testbedCsvFile);

        CSVFile2Wiseml wiseml = new CSVFile2Wiseml(testbedCsvFile);

    }


}

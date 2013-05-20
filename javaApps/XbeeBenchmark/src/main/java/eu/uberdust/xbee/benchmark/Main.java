package eu.uberdust.xbee.benchmark;

import eu.mksense.XBeeRadio;

/**
 * Created with IntelliJ IDEA.
 * User: amaxilatis
 * Date: 5/15/13
 * Time: 11:23 AM
 * To change this template use File | Settings | File Templates.
 */
public class Main {

    public static int main(final String[] args) {
        System.out.println("Booted up...");

        if (args.length < 3) {
            print_help_info();
            return 0;
        }

        String xbeePort = args[0];
        String positionMark = args[1];
        String xbeeRate = args[2];

        try {
            XBeeRadio.getInstance().open(xbeePort, Integer.parseInt(xbeeRate));
        } catch (final Exception e) {
            e.printStackTrace();
            return -1;
        }
        XbeeMessageLogger xbeelog = new XbeeMessageLogger(positionMark + ".data");
        XBeeRadio.getInstance().addMessageListener(112, xbeelog);

        return 0;
    }

    private static void print_help_info() {
        System.out.println("Usage:");
    }
}

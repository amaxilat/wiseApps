package eu.amaxilatis.csv2xml;

import eu.wisebed.wisedb.HibernateUtil;
import eu.wisebed.wisedb.controller.NodeController;
import eu.wisebed.wisedb.controller.TestbedController;
import eu.wisebed.wisedb.model.Testbed;
import eu.wisebed.wiseml.model.setup.Node;
import eu.wisebed.wiseml.model.setup.Position;
import eu.wisebed.wiseml.model.setup.Setup;
import org.apache.log4j.Logger;
import org.hibernate.Transaction;

/**
 * Created by IntelliJ IDEA.
 * User: amaxilatis
 * Date: 10/16/11
 * Time: 9:26 PM
 */
class WisedbUpdater {
    private static final Logger log = Logger.getLogger(WisedbUpdater.class);

    private final CsvTestbed csvTestbed;

    WisedbUpdater(String testbed_name, CsvTestbed csvTestbed) {
        this.csvTestbed = csvTestbed;
        check();
    }

    private void check() {
        // Initialize hibernate
        HibernateUtil.connectEntityManagers();

        for (String nodeId : csvTestbed.getNodes()) {
            log.debug(nodeId);
            CsvNode csvNode = csvTestbed.getNode(nodeId);
            Transaction tx = HibernateUtil.getInstance().getSession().beginTransaction();

            Node cnode = NodeController.getInstance().getByID(csvNode.id());
            if (cnode != null) {

                log.debug(csvNode.id());


                Position p = cnode.getPosition();
                try {
                    p.setX(csvNode.getX());
                    p.setY(csvNode.getY());
                    p.setZ(csvNode.getZ());
                    cnode.setPosition(p);


                    cnode.setDescription(csvNode.getDescrioption());
                    log.info("updating node " + cnode.getId());
                    log.debug(cnode.getId() + "," + cnode.getPosition().getX() + "," + cnode.getPosition().getY() + "," + cnode.getPosition().getZ());
                    log.debug(cnode.getDescription());
                } catch (NullPointerException e) {

                    log.error("getPosition returned null value for node " + csvNode.id());
                }
            } else {

                log.info("node does not exist, trying to add...");


                final String testbedPrefix = csvNode.id().substring(0, csvNode.id().indexOf("0"));
                log.debug("searching for testbed by urn: " + testbedPrefix);
                final Testbed tbed = TestbedController.getInstance().getByUrnPrefix(testbedPrefix);
                if (tbed != null) {
                    log.debug("need to place node in " + tbed.getId());
                    final Setup setup = tbed.getSetup();
                    if (tbed != null) {
                        log.debug(" for setup " + setup.getId());
                        Node newNode = new Node();
                        newNode.setId(csvNode.id());
                        newNode.setDescription(csvNode.getDescrioption());
                        newNode.setGateway(csvNode.gateway());
                        newNode.setNodeType(csvNode.nodeType());
                        Position position = new Position();
                        position.setX(csvNode.getX());
                        position.setY(csvNode.getY());
                        position.setZ(csvNode.getZ());
                        position.setPhi(0);
                        position.setTheta(0);
                        newNode.setPosition(position);
                        newNode.setProgramDetails("");
                        newNode.setSetup(setup);
                        NodeController.getInstance().add(newNode);
                        log.info("added node " + newNode.toString());
                    } else {
                        log.error("no setup for testbed with id:" + tbed.getId());
                    }
                } else {
                    log.error("no testbed for urnprefix:" + testbedPrefix);
                }


            }

            tx.commit();

            HibernateUtil.getInstance().closeSession();


        }
    }

}

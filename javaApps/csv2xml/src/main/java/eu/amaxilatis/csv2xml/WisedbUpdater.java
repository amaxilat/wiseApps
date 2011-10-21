package eu.amaxilatis.csv2xml;

import eu.wisebed.wisedb.HibernateUtil;
import eu.wisebed.wisedb.controller.NodeController;
import eu.wisebed.wiseml.model.setup.Position;
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

    private final Testbed testbed;

    WisedbUpdater(String testbed_name, Testbed testbed) {
        this.testbed = testbed;
        check();
    }

    private void check() {
        // Initialize hibernate
        HibernateUtil.connectEntityManagers();

        for (String nodeId : testbed.getNodes()) {
            log.debug(nodeId);
            Node node = testbed.getNode(nodeId);
            Transaction tx = HibernateUtil.getInstance().getSession().beginTransaction();

            eu.wisebed.wiseml.model.setup.Node cnode = NodeController.getInstance().getByID(node.id());
            if (cnode != null) {

                log.debug(node.id());


                Position p = cnode.getPosition();
                try {
                    p.setX(node.getX());
                    p.setY(node.getY());
                    p.setZ(node.getZ());
                    cnode.setPosition(p);


                    cnode.setDescription(node.getDescrioption());
                    log.info("updating node " + cnode.getId());
                    log.debug(cnode.getId() + "," + cnode.getPosition().getX() + "," + cnode.getPosition().getY() + "," + cnode.getPosition().getZ());
                    log.debug(cnode.getDescription());
                } catch (NullPointerException e) {
                    log.error("getPosition returned null value for node " + node.id());
                }
            } else {
                log.error("Cannot find node!!!");
            }

            tx.commit();

            HibernateUtil.getInstance().closeSession();


        }
    }

}

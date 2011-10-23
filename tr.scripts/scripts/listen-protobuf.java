import java.util.*;
import java.nio.*;

import javax.xml.datatype.DatatypeFactory;
import javax.xml.datatype.XMLGregorianCalendar;

import eu.wisebed.testbed.api.rs.RSServiceHelper;
import eu.wisebed.api.rs.PublicReservationData;
import eu.wisebed.api.rs.RS;

import eu.wisebed.api.snaa.SNAA;
import eu.wisebed.api.snaa.AuthenticationTriple;
import eu.wisebed.api.snaa.SecretAuthenticationKey;
import eu.wisebed.testbed.api.snaa.helpers.SNAAServiceHelper;

import eu.wisebed.testbed.api.wsn.WSNServiceHelper;
import eu.wisebed.api.controller.*;
import eu.wisebed.api.common.*;
import eu.wisebed.api.sm.*;
import eu.wisebed.api.wsn.*;

import de.uniluebeck.itm.tr.util.*;
import de.itm.uniluebeck.tr.wiseml.WiseMLHelper;

import de.uniluebeck.itm.wisebed.cmdlineclient.*;
import de.uniluebeck.itm.wisebed.cmdlineclient.wrapper.*;
import java.util.concurrent.Future;
import com.google.common.collect.*;

import de.uniluebeck.itm.wisebed.cmdlineclient.protobuf.*;

import java.util.Date;

//--------------------------------------------------------------------------
// Configuration
//--------------------------------------------------------------------------

	String secretReservationKeys = System.getProperty("testbed.secretreservationkeys");
	String pccHost = System.getProperty("testbed.protobuf.hostname");
	Integer pccPort = Integer.parseInt(System.getProperty("testbed.protobuf.port"));

         private BufferedWriter out;

//--------------------------------------------------------------------------
// Application logic
//--------------------------------------------------------------------------

	ProtobufControllerClient pcc = ProtobufControllerClient.create(pccHost, pccPort, helper.parseSecretReservationKeys(secretReservationKeys));
	pcc.addListener(new ProtobufControllerClientListener() {
		public void receive(List msgs) {
                      //log.debug("Received messages: " + msgs.size());
			for (int i=0; i<msgs.size(); i++) {
				Message msg = (Message) msgs.get(i);
                                synchronized(System.out) {
                                        String s = helper.toString(msg,true);
                                        log.debug(s);
//					log.debug(msg.write_packet() );
                                        out.write(s+"\n");
                                        out.flush();
                                }
			}
		}
		public void receiveStatus(List requestStatuses) {
			// nothing to do
		}
		public void receiveNotification(List msgs) {
			for (int i=0; i<msgs.size(); i++) {
				log.info(msgs.get(i));
			}
		}
		public void experimentEnded() {
			log.info("Experiment ended");
                        out.close();
			System.exit(0);
		}
		public void onConnectionEstablished() {
			Date d = new Date();
//                        out = new BufferedWriter(new FileWriter("../traces/trace."+d.getTime() ));
                        out = new BufferedWriter(new FileWriter("../traces/trace.out" ));
			log.debug("Connection established.");
		}
		public void onConnectionClosed() {
			log.debug("Connection closed.");
		}
	});
	pcc.connect();
	

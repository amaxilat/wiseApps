import java.util.*;
import java.nio.*;

import javax.xml.datatype.DatatypeFactory;
import javax.xml.datatype.XMLGregorianCalendar;
import java.math.BigInteger;

import eu.wisebed.testbed.api.rs.RSServiceHelper;
import eu.wisebed.testbed.api.rs.v1.PublicReservationData;
import eu.wisebed.testbed.api.rs.v1.RS;

import eu.wisebed.testbed.api.snaa.v1.SNAA;
import eu.wisebed.testbed.api.snaa.v1.AuthenticationTriple;
import eu.wisebed.testbed.api.snaa.v1.SecretAuthenticationKey;
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



//--------------------------------------------------------------------------
// Configuration
//--------------------------------------------------------------------------

	// Endpoint URL of local controller instance, the testbed will use this URL to send us node outputs
	String localControllerEndpointURL	= "http://" + InetAddress.getLocalHost().getCanonicalHostName() + ":8089/controller";
	String secretReservationKeys = System.getProperty("testbed.secretreservationkeys");
	String messageToSend = System.getProperty("testbed.message");
	String nodeUrnToFlash = System.getProperty("testbed.nodeurns");

	// Endpoint URLs of Authentication (SNAA), Reservation (RS) and Experimentation (iWSN) services
	String sessionManagementEndpointURL	= System.getProperty("testbed.sm.endpointurl");

	// Retrieve Java proxies of the endpoint URLs above
	SessionManagement sessionManagement = WSNServiceHelper.getSessionManagementService(sessionManagementEndpointURL);


//--------------------------------------------------------------------------
// Application logic
//--------------------------------------------------------------------------

	log.debug("Using the following parameters for calling getInstance(): {}, {}",
			StringUtils.jaxbMarshal(helper.parseSecretReservationKeys(secretReservationKeys)),
			localControllerEndpointURL
	);

	Controller controller = new Controller() {
		public void receive(List msg) {
			// nothing to do
		}
		public void receiveStatus(List requestStatuses) {
			wsn.receive(requestStatuses);
		}
		public void receiveNotification(List msgs) {
			for (int i=0; i<msgs.size(); i++) {
				log.info(msgs.get(i));
			}
		}
		public void experimentEnded() {
			log.info("Experiment ended");
			System.exit(0);
		}
	};

	DelegatingController delegator = new DelegatingController(controller);
	delegator.publish(localControllerEndpointURL);
	log.info("Local controller published on url: {}", localControllerEndpointURL);

	String wsnEndpointURL = null;
	try {
		wsnEndpointURL = sessionManagement.getInstance(
				helper.parseSecretReservationKeys(secretReservationKeys),
				localControllerEndpointURL
		);
	} catch (UnknownReservationIdException_Exception e) {
		log.warn("There was not reservation found with the given secret reservation key. Exiting.");
		System.exit(1);
	}

	log.info("Got a WSN instance URL, endpoint is: {}", wsnEndpointURL);
	WSN wsnService = WSNServiceHelper.getWSNService(wsnEndpointURL);
	final WSNAsyncWrapper wsn = WSNAsyncWrapper.of(wsnService);

	// retrieve reserved node URNs from testbed
	List nodeURNs;
	if (nodeUrnToFlash != null && !"".equals(nodeUrnToFlash)) {
		nodeURNs = Lists.newArrayList(nodeUrnToFlash.split(","));
	} else {
		nodeURNs = WiseMLHelper.getNodeUrns(wsn.getNetwork().get(), new String[]{});
	}
	log.info("Retrieved the following node URNs: {}", nodeURNs);

	// Constructing UART Message from Input String (Delimited by ",")
	// Supported Prefixes are "0x" and "0b", otherwise Base_10 (DEZ) is assumed	
	String[] splitMessage = messageToSend.split(",");
	byte[] messageToSendBytes = new byte[splitMessage.length];
	String messageForOutputInLog = "";
	for (int i=0;i<splitMessage.length;i++) {
		int type = 10;
		if (splitMessage[i].startsWith("0x")) {
			type = 16;
			splitMessage[i]=splitMessage[i].replace("0x","");
		} else if (splitMessage[i].startsWith("0b")) {
			type = 2;
			splitMessage[i]=splitMessage[i].replace("0b","");
		}
		BigInteger b = new BigInteger(splitMessage[i], type);
		messageToSendBytes[i] = (byte) b.intValue() ;
		messageForOutputInLog = messageForOutputInLog + b.intValue() +" ";
	}
	
	log.info("Sending Message [ "+messageForOutputInLog+"] to nodes...");
	
	// Constructing the Message
	Message binaryMessage = new Message();
	binaryMessage.setBinaryData(messageToSendBytes);
	
	GregorianCalendar c = new GregorianCalendar();
	c.setTimeInMillis(System.currentTimeMillis());

	binaryMessage.setTimestamp(DatatypeFactory.newInstance().newXMLGregorianCalendar(c));
	binaryMessage.setSourceNodeId("urn:wisebed:uzl1:0xFFFF");
	
	Future sendFuture = wsn.send(nodeURNs, binaryMessage, 3, TimeUnit.MINUTES);
	JobResult sendJobResult = sendFuture.get();
	log.info("{}", sendJobResult);

	
	log.info("Shutting down...");
	System.exit(0);

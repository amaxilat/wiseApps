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

//--------------------------------------------------------------------------
// Configuration
//--------------------------------------------------------------------------

	String secretReservationKeys = System.getProperty("testbed.secretreservationkeys");
	String sessionManagementEndpointURL	= System.getProperty("testbed.sm.endpointurl");
	//String nodeUrnsToReset = System.getProperty("testbed.nodeurns");

	String pccHost = System.getProperty("testbed.protobuf.hostname");
	Integer pccPort = Integer.parseInt(System.getProperty("testbed.protobuf.port"));

	SessionManagement sessionManagement = WSNServiceHelper.getSessionManagementService(sessionManagementEndpointURL);

//--------------------------------------------------------------------------
// Application logic
//--------------------------------------------------------------------------

	String wsnEndpointURL = null;
	try {
		wsnEndpointURL = sessionManagement.getInstance(
				helper.parseSecretReservationKeys(secretReservationKeys),
				"NONE"
		);
	} catch (UnknownReservationIdException_Exception e) {
		log.warn("There was not reservation found with the given secret reservation key. Exiting.");
		System.exit(1);
	}

	log.info("Got a WSN instance URL, endpoint is: {}", wsnEndpointURL);
	WSN wsnService = WSNServiceHelper.getWSNService(wsnEndpointURL);
	final WSNAsyncWrapper wsn = WSNAsyncWrapper.of(wsnService);

	ProtobufControllerClient pcc = ProtobufControllerClient.create(pccHost, pccPort, helper.parseSecretReservationKeys(secretReservationKeys));
	pcc.addListener(new ProtobufControllerClientListener() {
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
		public void onConnectionEstablished() {
			log.debug("Connection established.");
		}
		public void onConnectionClosed() {
			log.debug("Connection closed.");
		}
	});
	pcc.connect();



	//Setting the demands

	List demandNodeURNs;
	demandNodeURNs = WiseMLHelper.getNodeUrns(wsn.getNetwork().get(), new String[]{});
	log.info("Setting Demands for the following node URNs: {}", demandNodeURNs);
	int[] demand_semantics = new int [] {4,211};
	int[] demand_values = new int [] {0xff,55};


	// Send a message to nodes via uart (to receive them enable RX_UART_MSGS in the fronts_config.h-file)
	Message msg = new Message();
	byte[] binary_data = new byte[4+8*demand_semantics.length];
	binary_data[0]=(byte)0xa;
	binary_data[1]=(byte)2;
	binary_data[2]=(byte)7;
	binary_data[3]=(byte)1;
	
	for (int i=0;i<demand_semantics.length;i++){
		binary_data[3+1+i*8]=(byte)0;
		binary_data[3+2+i*8]=(byte)0;
		binary_data[3+3+i*8]=(byte)0;
		binary_data[3+4+i*8]=(byte)demand_semantics[i];
		binary_data[3+5+i*8]=(byte)0;
		binary_data[3+6+i*8]=(byte)0;
		binary_data[3+7+i*8]=(byte)0;
		binary_data[3+8+i*8]=(byte)demand_values[i];
	}

	msg.setBinaryData(binary_data);
	msg.setSourceNodeId("urn:wisebed:ctitestbed:0xffff");
	msg.setTimestamp(DatatypeFactory.newInstance().newXMLGregorianCalendar((GregorianCalendar)GregorianCalendar.getInstance()));

	Future sendFuture = wsn.send(demandNodeURNs,msg, 10, TimeUnit.SECONDS);
	JobResult sendJobResult = sendFuture.get();
	log.info("{}", sendJobResult);
	if (sendJobResult.getSuccessPercent() < 100) {
		System.out.println("Not all nodes could receive the message. Exiting");
		System.exit(1);
	}

	log.info("Closing connection...");
	pcc.disconnect();

	log.info("Shutting down...");
	System.exit(0);

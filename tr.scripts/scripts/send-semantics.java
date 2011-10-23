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

	// Endpoint URL of local controller instance, the testbed will use this URL to send us node outputs
//	String localControllerEndpointURL	= "http://" + InetAddress.getLocalHost().getCanonicalHostName() + ":8089/controller";
        String localControllerEndpointURL       = System.getProperty("local.endpoint")+":"+System.getProperty("local.portcommand");

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



	String[] nodeUrns4Semantics= new String [] {
		//TUBS Sensors
		//BUILDING 1
		"urn:wisebed:tubs:401,urn:wisebed:tubs:402,urn:wisebed:tubs:403,urn:wisebed:tubs:405,urn:wisebed:tubs:407,urn:wisebed:tubs:408,urn:wisebed:tubs:410,urn:wisebed:tubs:412,urn:wisebed:tubs:414,urn:wisebed:tubs:417,urn:wisebed:tubs:418,urn:wisebed:tubs:421,urn:wisebed:tubs:422,urn:wisebed:tubs:424,urn:wisebed:tubs:428",
		//FLOOR 2
		"urn:wisebed:tubs:401,urn:wisebed:tubs:402,urn:wisebed:tubs:403,urn:wisebed:tubs:405,urn:wisebed:tubs:407,urn:wisebed:tubs:408,urn:wisebed:tubs:410,urn:wisebed:tubs:412,urn:wisebed:tubs:414,urn:wisebed:tubs:417,urn:wisebed:tubs:418,urn:wisebed:tubs:421,urn:wisebed:tubs:422,urn:wisebed:tubs:424,urn:wisebed:tubs:428",
		//PIR 1
		"urn:wisebed:tubs:401,urn:wisebed:tubs:402,urn:wisebed:tubs:403,urn:wisebed:tubs:405,urn:wisebed:tubs:407,urn:wisebed:tubs:408,urn:wisebed:tubs:410,urn:wisebed:tubs:412,urn:wisebed:tubs:414,urn:wisebed:tubs:417,urn:wisebed:tubs:418,urn:wisebed:tubs:421,urn:wisebed:tubs:422,urn:wisebed:tubs:424,urn:wisebed:tubs:428",
		//CTI
		//BUILDING 2
		"urn:wisebed:ctitestbed:0x296,urn:wisebed:ctitestbed:0xcaa,urn:wisebed:ctitestbed:0x1c98,urn:wisebed:ctitestbed:0x295,urn:wisebed:ctitestbed:0x1b77,urn:wisebed:ctitestbed:0x9979,urn:wisebed:ctitestbed:0x153d,urn:wisebed:ctitestbed:0x585,urn:wisebed:ctitestbed:0x786a,urn:wisebed:ctitestbed:0xca3,urn:wisebed:ctitestbed:0x1cde",
		//FLOOR 0
		"urn:wisebed:ctitestbed:0x296,urn:wisebed:ctitestbed:0xcaa,urn:wisebed:ctitestbed:0x1c98,urn:wisebed:ctitestbed:0x295,urn:wisebed:ctitestbed:0x1b77,urn:wisebed:ctitestbed:0x9979,urn:wisebed:ctitestbed:0x153d,urn:wisebed:ctitestbed:0x585,urn:wisebed:ctitestbed:0x786a,urn:wisebed:ctitestbed:0xca3,urn:wisebed:ctitestbed:0x1cde",
		//ROOM 11
		"urn:wisebed:ctitestbed:0x296",
		//ROOM 9
		"urn:wisebed:ctitestbed:0xcaa,urn:wisebed:ctitestbed:0x1c98,urn:wisebed:ctitestbed:0x9979,urn:wisebed:ctitestbed:0x153d",
		//ROOM 2
		"urn:wisebed:ctitestbed:0x295,urn:wisebed:ctitestbed:0x1b77",
		//ROOM 3
		"urn:wisebed:ctitestbed:0x585,urn:wisebed:ctitestbed:0x786a",		
		//ROOM 1
		"urn:wisebed:ctitestbed:0xca3,urn:wisebed:ctitestbed:0x1cde",
		//TEMP 
		"urn:wisebed:ctitestbed:0x296,urn:wisebed:ctitestbed:0xcaa,urn:wisebed:ctitestbed:0x1c98,urn:wisebed:ctitestbed:0x295,urn:wisebed:ctitestbed:0x1b77,urn:wisebed:ctitestbed:0x9979,urn:wisebed:ctitestbed:0x153d,urn:wisebed:ctitestbed:0x585,urn:wisebed:ctitestbed:0x786a,urn:wisebed:ctitestbed:0xca3,urn:wisebed:ctitestbed:0x1cde",
		//LIGHT
		"urn:wisebed:ctitestbed:0x296,urn:wisebed:ctitestbed:0xcaa,urn:wisebed:ctitestbed:0x1c98,urn:wisebed:ctitestbed:0x295,urn:wisebed:ctitestbed:0x1b77,urn:wisebed:ctitestbed:0x9979,urn:wisebed:ctitestbed:0x153d,urn:wisebed:ctitestbed:0x585,urn:wisebed:ctitestbed:0x786a,urn:wisebed:ctitestbed:0xca3,urn:wisebed:ctitestbed:0x1cde",
		//PIR
		"urn:wisebed:ctitestbed:0x9979",
		//UNIGE
		//Room 1
		"urn:wisebed:unige:0x9710,urn:wisebed:unige:0x1b7f,urn:wisebed:unige:0x99ad,urn:wisebed:unige:0x96e5,urn:wisebed:unige:0x999d",
		//room 2
		"urn:wisebed:unige:0x1b95,urn:wisebed:unige:0x15d3,urn:wisebed:unige:0x9731,urn:wisebed:unige:0x96c2,urn:wisebed:unige:0x9700,urn:wisebed:unige:0x1725",
		//room 3
		"urn:wisebed:unige:0x96fc,urn:wisebed:unige:0x96f4,urn:wisebed:unige:0x99bd,urn:wisebed:unige:0x15e0,urn:wisebed:unige:0x1b3b,urn:wisebed:unige:0x1520",
		//room 4
		"urn:wisebed:unige:0x1b4c,urn:wisebed:unige:0x1cb9,urn:wisebed:unige:0x1b84,urn:wisebed:unige:0x9708,urn:wisebed:unige:0x96e0",
		//room 5
		"urn:wisebed:unige:0x9a0d,urn:wisebed:unige:0x1b99,urn:wisebed:unige:0x96eb,urn:wisebed:unige:0x1be5,urn:wisebed:unige:0x9a0c",
		//building
		"urn:wisebed:unige:0x9710,urn:wisebed:unige:0x1b7f,urn:wisebed:unige:0x99ad,urn:wisebed:unige:0x96e5,urn:wisebed:unige:0x999d,urn:wisebed:unige:0x1b95,urn:wisebed:unige:0x15d3,urn:wisebed:unige:0x9731,urn:wisebed:unige:0x96c2,urn:wisebed:unige:0x9700,urn:wisebed:unige:0x1725,urn:wisebed:unige:0x96fc,urn:wisebed:unige:0x96f4,urn:wisebed:unige:0x99bd,urn:wisebed:unige:0x15e0,urn:wisebed:unige:0x1b3b,urn:wisebed:unige:0x1520,urn:wisebed:unige:0x1b4c,urn:wisebed:unige:0x1cb9,urn:wisebed:unige:0x1b84,urn:wisebed:unige:0x9708,urn:wisebed:unige:0x96e0,urn:wisebed:unige:0x9a0d,urn:wisebed:unige:0x1b99,urn:wisebed:unige:0x96eb,urn:wisebed:unige:0x1be5,urn:wisebed:unige:0x9a0c",
		//floor
		"urn:wisebed:unige:0x9710,urn:wisebed:unige:0x1b7f,urn:wisebed:unige:0x99ad,urn:wisebed:unige:0x96e5,urn:wisebed:unige:0x999d,urn:wisebed:unige:0x1b95,urn:wisebed:unige:0x15d3,urn:wisebed:unige:0x9731,urn:wisebed:unige:0x96c2,urn:wisebed:unige:0x9700,urn:wisebed:unige:0x1725,urn:wisebed:unige:0x96fc,urn:wisebed:unige:0x96f4,urn:wisebed:unige:0x99bd,urn:wisebed:unige:0x15e0,urn:wisebed:unige:0x1b3b,urn:wisebed:unige:0x1520,urn:wisebed:unige:0x1b4c,urn:wisebed:unige:0x1cb9,urn:wisebed:unige:0x1b84,urn:wisebed:unige:0x9708,urn:wisebed:unige:0x96e0,urn:wisebed:unige:0x9a0d,urn:wisebed:unige:0x1b99,urn:wisebed:unige:0x96eb,urn:wisebed:unige:0x1be5,urn:wisebed:unige:0x9a0c",
	};
	int[] semantics = new int [] {
		//TUBS Sensors		
		1, //BUILDING
		2, //FLOOR
		212, //PIR
		//CTI
		1,//BUILDING 2
		2,//FLOOR 0
		4,//ROOM
		4,//ROOM
		4,//ROOM
		4,//ROOM
		4,//ROOM
		210,//TEMP 
		211,//LIGHT
		212,//PIR
		//UNIGE
		4,		4,		4,		4,		4, //ROOM
		1, // BUILDING
		2 //FLOOR
		
		};
	int[] values = new int [] {
		//TUBS Sensors
		1, //BUILDING
		2, //FLOOR
		1, //PIR
		//CTI
		2,//BUILDING 2
		0,//FLOOR 0
		11,		9,		2,		3,		1,//ROOM
		0,//TEMP 
		0,//LIGHT
		0,//PIR
		//UNIGE
		1,2,3,4,5, //rooms
		3, //building
		1 //floor
		};
	String tbed_prefix= "";

	//String nodeUrnsToReset = "urn:wisebed:ctitestbed:0x1ccd,urn:wisebed:ctitestbed:0x296,urn:wisebed:ctitestbed:0xcaa,urn:wisebed:ctitestbed:0x1c98,urn:wisebed:ctitestbed:0x295,urn:wisebed:ctitestbed:0x1b77,urn:wisebed:ctitestbed:0x9979,urn:wisebed:ctitestbed:0x153d,urn:wisebed:ctitestbed:0x585,urn:wisebed:ctitestbed:0x786a,urn:wisebed:ctitestbed:0xca3,urn:wisebed:ctitestbed:0x1cde";	
	for (int i=0;i<nodeUrns4Semantics.length;i++){
		// retrieve reserved node URNs from testbed
		List nodeURNs;
		nurn = nodeUrns4Semantics[i].split(",");
		for (int j=0;j<nurn.length;j++){
			nurn[j]=tbed_prefix+nurn[j];
		}
		nodeURNs = Lists.newArrayList(nurn);
		log.info("Retrieved the following node URNs: {}", nodeURNs);

		log.info("Sending the semantics... {}-{}",semantics[i],values[i]);

		// Send a message to nodes via uart (to receive them enable RX_UART_MSGS in the fronts_config.h-file)
		Message msg = new Message();
		//BinaryMessage bmsg = new BinaryMessage();
		msg.setBinaryData(new byte[]{0xa,0x2,0x6,0,0,0,semantics[i],0,0,0,values[i] }); //command (1 = hello module) and sub command
		msg.setSourceNodeId("urn:wisebed:ctitestbed:0xffff");
		msg.setTimestamp(DatatypeFactory.newInstance().newXMLGregorianCalendar((GregorianCalendar)GregorianCalendar.getInstance()));
		try{
		Future sendFuture = wsn.send(nodeURNs,msg, 10, TimeUnit.SECONDS);
	
		JobResult sendJobResult = sendFuture.get();
		log.info("{}", sendJobResult);		
		Thread.sleep(250);
		}catch (Exception e){}
	}

	Thread.sleep(1000);
		List demandNodeURNs;
		demandNodeURNs = WiseMLHelper.getNodeUrns(wsn.getNetwork().get(), new String[]{});
		log.info("Setting Demands for the following node URNs: {}", demandNodeURNs);
		// Send a message to nodes via uart (to receive them enable RX_UART_MSGS in the fronts_config.h-file)
		Message msg = new Message();
		msg.setBinaryData(new byte[]{0xa,0x2,0x1});
		msg.setSourceNodeId("urn:wisebed:ctitestbed:0xffff");
		msg.setTimestamp(DatatypeFactory.newInstance().newXMLGregorianCalendar((GregorianCalendar)GregorianCalendar.getInstance()));

		Future sendFuture = wsn.send(demandNodeURNs,msg, 10, TimeUnit.SECONDS);
		JobResult sendJobResult = sendFuture.get();
		log.info("{}", sendJobResult);
		if (sendJobResult.getSuccessPercent() < 100) {
			System.out.println("Not all nodes could receive the message. Exiting");
			System.exit(1);
		}

	log.info("Shutting down...");
	System.exit(0);

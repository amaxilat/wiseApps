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
	String nodeUrnsToReset = System.getProperty("testbed.nodeurns");
	
	String pccHost = System.getProperty("testbed.protobuf.hostname");
	Integer pccPort = Integer.parseInt(System.getProperty("testbed.protobuf.port"));
	
	SessionManagement sessionManagement = WSNServiceHelper.getSessionManagementService(sessionManagementEndpointURL);

//--------------------------------------------------------------------------
// Application logic
//--------------------------------------------------------------------------
        String localControllerEndpointURL = "http://localhost:9088/controller";
   	source("../scripts/include/controller.java");
        MyController control = new MyController();
        DelegatingController delegator = new DelegatingController(control);
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

//        String wsnEndpointURL = sessionManagement.getInstance(
//                        helper.copyRsToWsn(secretReservationKeys),
 //                       localControllerEndpointURL
 //       );

//        log.info("Got an WSN instance URL, endpoint is: {}", wsnEndpointURL);
        //WSN wsn = WSNServiceHelper.getWSNService(wsnEndpointURL);

//		WSN wsnService = WSNServiceHelper.getWSNService(wsnEndpointURL);
//		final WSNAsyncWrapper wsn = WSNAsyncWrapper.of(wsnService);        

	// retrieve reserved node URNs from testbed
	List nodeURNs;
	if (nodeUrnsToReset != null && !"".equals(nodeUrnsToReset)) {
		nodeURNs = Lists.newArrayList(nodeUrnsToReset.split(","));
	} else {
		nodeURNs = WiseMLHelper.getNodeUrns(wsn.getNetwork().get(), new String[]{});
	}
	log.info("Retrieved the following node URNs: {}", nodeURNs);

	log.info("Resetting nodes...");
	
	// Send a message to nodes via uart (to receive them enable RX_UART_MSGS in the fronts_config.h-file)
	Message msg = new Message();
	//BinaryMessage bmsg = new BinaryMessage();
	System.out.println("Message contents "+ 0xa +" "+0x1 +" "+ Integer.parseInt(System.getProperty("testbed.command")) +" "+ Integer.parseInt(System.getProperty("testbed.kval")) );
	msg.setBinaryData(new byte[]{0xa,0x1,Integer.parseInt(System.getProperty("testbed.command")) , Integer.parseInt(System.getProperty("testbed.kval")) }); //command (1 = hello module) and sub command
	msg.setSourceNodeId("0");
	msg.setTimestamp(DatatypeFactory.newInstance().newXMLGregorianCalendar((GregorianCalendar)GregorianCalendar.getInstance()));

	Future sendFuture = wsn.send(nodeURNs,msg, 10, TimeUnit.SECONDS);
Thread.sleep(2000);	
//	JobResult sendJobResult = sendFuture.get();
//	log.info("{}", sendJobResult);
//	if (sendJobResult.getSuccessPercent() < 100) {
//		System.out.println("Not all nodes could receive the message. Exiting");
//		System.exit(1);
//	}

//	log.info("Closing connection...");
//	pcc.disconnect();
	
	log.info("Shutting down...");
	System.exit(0);
	

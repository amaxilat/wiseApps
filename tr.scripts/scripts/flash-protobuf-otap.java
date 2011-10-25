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
import com.coalesenses.isense.ishell.interpreter.IShellInterpreterSetChannelMessage;
import com.google.common.collect.Sets;
import com.google.common.io.Files;
import de.uniluebeck.itm.netty.handlerstack.FilterHandler;
import de.uniluebeck.itm.netty.handlerstack.FilterPipeline;
import de.uniluebeck.itm.netty.handlerstack.FilterPipelineImpl;
import de.uniluebeck.itm.netty.handlerstack.HandlerFactoryRegistry;
import de.uniluebeck.itm.netty.handlerstack.isenseotap.ISenseOtapAutomatedProgrammingRequest;
import de.uniluebeck.itm.netty.handlerstack.isenseotap.presencedetect.PresenceDetectControlStop;
import de.uniluebeck.itm.netty.handlerstack.protocolcollection.ProtocolCollection;
import de.uniluebeck.itm.nettyrxtx.RXTXChannelFactory;
import de.uniluebeck.itm.nettyrxtx.RXTXDeviceAddress;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.PosixParser;
import org.apache.log4j.*;
import org.jboss.netty.bootstrap.ClientBootstrap;
import org.jboss.netty.channel.*;
import org.jboss.netty.channel.group.ChannelGroup;
import org.jboss.netty.channel.group.DefaultChannelGroup;
import org.slf4j.LoggerFactory;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Set;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import de.uniluebeck.itm.netty.handlerstack.util.HeaderAndJavaBeansXMLDecoderEncoder;


//--------------------------------------------------------------------------
// Configuration
//--------------------------------------------------------------------------

	String secretReservationKeys = System.getProperty("testbed.secretreservationkeys");
	String imageToFlashPath = System.getProperty("testbed.image");
	String nodeUrnsToFlash = System.getProperty("testbed.nodeurns");

	String pccHost = System.getProperty("testbed.protobuf.hostname");
	Integer pccPort = Integer.parseInt(System.getProperty("testbed.protobuf.port"));

	String sessionManagementEndpointURL	= System.getProperty("testbed.sm.endpointurl");
	
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




// retrieve reserved node URNs from testbed
Set otapDevices = new HashSet();
otapDevices.add(0x1bbd);

byte[] otapProgram = Files.toByteArray(new File(System.getProperty("testbed.image")));
	
ISenseOtapAutomatedProgrammingRequest req = new ISenseOtapAutomatedProgrammingRequest(otapDevices,otapProgram);

Object serializedReq = HeaderAndJavaBeansXMLDecoderEncoder.encode(ISenseOtapAutomatedProgrammingRequest.SERIALIZATION_HEADER,req);


	Message msg = new Message();
	//BinaryMessage bmsg = new BinaryMessage();
	msg.setBinaryData(serializedReq.toString().getBytes() );
	msg.setSourceNodeId("urn:wisebed:ctitestbed:0xffff");
	msg.setTimestamp(DatatypeFactory.newInstance().newXMLGregorianCalendar((GregorianCalendar)GregorianCalendar.getInstance()));
	

// retrieve reserved node URNs from testbed
	List nodeURNs;
	nodeURNs = Lists.newArrayList("urn:wisebed:ctitestbed:0x9979".split(","));

	Future sendFuture = wsn.send(nodeURNs,msg, 10, TimeUnit.SECONDS);
	JobResult sendJobResult = sendFuture.get();
	log.info("{}", sendJobResult);
	if (sendJobResult.getSuccessPercent() < 100) {
		System.out.println("Not all nodes could receive the message. Exiting");		
	}

//while (true){
//Thread.sleep(1000);
//log.info(".");
//}

//	log.info("Closing connection...");
//	pcc.disconnect();
	
//	log.info("Shutting down...");
//	System.exit(0);


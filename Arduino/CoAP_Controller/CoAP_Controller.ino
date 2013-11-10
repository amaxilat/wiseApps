//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
//Include XBEE Libraries
#include <XBee.h>
#include <XbeeRadio.h>
#include <coap.h>
#include <BaseRouting.h>
#define PINSTART 2

int reading;

//Create the XbeeRadio object we'll be using
XBeeRadio xbee = XBeeRadio();
// create a reusable response object for responses we expect to handle 
XBeeRadioResponse response = XBeeRadioResponse();

int switchCount=4;

uint16_t devices[4]={
  0x2eb,0x319,0x482,0x328};

void setup(){
pinMode(13,OUTPUT);
digitalWrite(13,HIGH);
delay(100);
digitalWrite(13,LOW);
delay(100);
digitalWrite(13,HIGH);
delay(100);
digitalWrite(13,LOW);
delay(100);
  
  for (int i=0;i<switchCount;i++){
    pinMode(PINSTART+i, INPUT);
  }

  xbee.initialize_xbee_module();
  //start our XbeeRadio object and set our baudrate to 38400.
  xbee.begin(38400);
  //Initialize our XBee module with the correct values using channel 13
  xbee.init(12); 
}

void loop(){
digitalWrite(13,HIGH);
delay(100);
digitalWrite(13,LOW);
delay(100);

  for (int i=0;i<switchCount;i++){
    if (digitalRead(PINSTART+i)==LOW){
      char name [4];
      sprintf(name,"%x",devices[i]);

      coap_packet_t packet = post_coap_message("lz1", name,"1");
      uint8_t buf[100];
      uint8_t size = packet.packet_to_buffer(buf);
      // create a tx16 request object
      Tx16Request tx = Tx16Request(devices[i], &(buf[0]), size);
      //Send our package using port 112 instead of the default one (110)
      xbee.send(tx, 112);    
    }
    else{
      char name [4];
      sprintf(name,"%x",devices[i]);

      coap_packet_t packet = post_coap_message("lz1", name,"0");
      uint8_t buf[100];
      uint8_t size = packet.packet_to_buffer(buf);
      // create a tx16 request object
      Tx16Request tx = Tx16Request(devices[i], &(buf[0]), size);
      //Send our package using port 112 instead of the default one (110)
      xbee.send(tx, 112);    
    }
  }
  delay(1000);
}






























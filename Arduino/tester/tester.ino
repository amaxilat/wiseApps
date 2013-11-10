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

int switchCount=1;

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

Serial.begin(38400);
}

void loop(){
digitalWrite(13,HIGH);
delay(10);
digitalWrite(13,LOW);
delay(10);
      Serial.println("==================");

  for (int i=0;i<switchCount;i++){
    int value=digitalRead(PINSTART+i);
    if (value==HIGH){
      char name [10];
      sprintf(name,"%x,ON{%d}",devices[i],value);
      Serial.println(name);
    }
    else{
      char name [10];
      sprintf(name,"%x,OFF{%d}",devices[i],value);
      Serial.println(name);
    }
  }
  delay(100);
}






























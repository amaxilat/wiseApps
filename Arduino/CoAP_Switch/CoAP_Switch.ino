//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
//Include XBEE Libraries
#include <XBee.h>
#include <XbeeRadio.h>
#include <coap.h>
#include <BaseRouting.h>

/*******************************************************
 * 
 * This program will test the LCD panel and the buttons
 * Mark Bramwell, July 2010
 * 
 ********************************************************/

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

char zones[20][10];
int czone;
int mcount;
uint16_t sender;
long lastUpdate;

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// read the buttons
int read_LCD_buttons()
{
  adc_key_in = analogRead(0);      // read the value from the sensor 
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  // For V1.1 us this threshold
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 250)  return btnUP; 
  if (adc_key_in < 450)  return btnDOWN; 
  if (adc_key_in < 650)  return btnLEFT; 
  if (adc_key_in < 850)  return btnSELECT;  

  // For V1.0 comment the other threshold and use the one below:
  /*
 if (adc_key_in < 50)   return btnRIGHT;  
   if (adc_key_in < 195)  return btnUP; 
   if (adc_key_in < 380)  return btnDOWN; 
   if (adc_key_in < 555)  return btnLEFT; 
   if (adc_key_in < 790)  return btnSELECT;   
   */


  return btnNONE;  // when all others fail, return this...
}



//Create the XbeeRadio object we'll be using
XBeeRadio xbee = XBeeRadio();
// create a reusable response object for responses we expect to handle 
XBeeRadioResponse response = XBeeRadioResponse();

void setup()
{
  lastUpdate=millis();
  sender=0xffff;
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0,0);
  lcd.print("loading grub..."); // print a simple message

  xbee.initialize_xbee_module();
  //  //start our XbeeRadio object and set our baudrate to 38400.
  xbee.begin(38400);
  //  //Initialize our XBee module with the correct values using channel 13
  xbee.init(12); 

  for (int i=0;i<20;i++)
    strcpy(zones[i],"");

  czone=0;
  mcount=0;
  lcd.setCursor(0,0);
}

void loop()
{

  //returns true if there is a packet for us on port 112
  if(xbee.checkForData(112))
  {
    Rx16Response rx;
    //get our response and save it on our response variable
    xbee.getResponse().getRx16Response(rx);
    uint16_t tsender = rx.getRemoteAddress16();

    if (tsender ==0x2eb){
      int dlen = xbee.getResponse().getDataLength();
      uint8_t data[dlen];
      memcpy(data,xbee.getResponse().getData(), dlen);

      if (dlen==7){
        sender=tsender;
        if (strncmp("hereiam",(char*)data,7)==0){
          coap_packet_t packet = get_coap_message(".well-known/core", sender);

          uint8_t buf[100];
          uint8_t size = packet.packet_to_buffer(buf);

          // create a tx16 request object
          Tx16Request tx = Tx16Request(sender, &(buf[0]), size);

          //Send our package using port 112 instead of the default one (110)
          xbee.send(tx, 112);

        }
      }
      else{
        if (tsender==sender){
          coap_packet_t packet;
          packet.init();
          uint8_t helperBuf_[100];
          packet.buffer_to_packet(dlen, data, (char*) helperBuf_);
          if (packet.mid_w()==1){
            size_t payloadLen = packet.payload_len_w();
            char *payload = (char*)packet.payload_w();

            char *str;

            while ((str = strtok_r(payload, ",", &payload)) != NULL){ // delimiter is the semicolon
              strncpy(zones[mcount++],&(str[1]),strlen(str)-2);
            }
          }
        }
      }
    }
  } 

  if (millis()-lastUpdate>500){
    // clear the screen
    lcd.clear();
    lcd.setCursor(0,0);            // move to the begining of the second line
    char senderChar[12];
    sprintf(senderChar , "dev:%x %s",sender,zones[czone]);
    lcd.print(senderChar);
    lastUpdate=millis();
  }

  lcd.setCursor(0,1);            // move to the begining of the second line
  lcd_key = read_LCD_buttons();  // read the buttons

    switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
  case btnRIGHT:
    {
      coap_packet_t packet = post_coap_message(zones[czone], "2eb","1");
      uint8_t buf[100];
      uint8_t size = packet.packet_to_buffer(buf);

      // create a tx16 request object
      Tx16Request tx = Tx16Request(sender, &(buf[0]), size);

      //Send our package using port 112 instead of the default one (110)
      xbee.send(tx, 112);
      break;
    }
  case btnLEFT:
    {


      coap_packet_t packet1 = post_coap_message(zones[czone], "2eb","0");

      uint8_t buf1[100];
      uint8_t size1 = packet1.packet_to_buffer(buf1);

      // create a tx16 request object
      Tx16Request tx1 = Tx16Request(sender, &(buf1[0]), size1);

      //Send our package using port 112 instead of the default one (110)
      xbee.send(tx1, 112);

      break;
    }
  case btnUP:
    {
      czone++;
      if (czone==mcount+1){
        czone=mcount;
      }
      delay(200);
      break;
    }
  case btnDOWN:
    {
      czone--;
      if (czone==-1){
        czone=0;
      }
      delay(200);
      break;
    }
  case btnSELECT:
    {
      //      lcd.print("SELECT");
      break;
    }
  case btnNONE:
    {
      lcd.print("<-off|on->");
      break;
    }
  }


}

























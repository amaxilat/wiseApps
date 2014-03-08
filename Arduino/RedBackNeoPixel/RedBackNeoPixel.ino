// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 6

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
//NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 2, 1, PIN,
NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS,
NEO_GRB + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), 
  matrix.Color(0, 255, 0), 
  matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 0),
  matrix.Color(160, 32, 240),
  matrix.Color(255, 222, 173) };

String printedMessage = "P-Space";
String message = "P-Space";
uint16_t newColor = 0;



/*
 * A simple sketch that uses WiServer to serve a web page
 */

#include <WiShield.h>
#include <WiServer.h>

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {
  192,168,1,8};	// IP address of WiShield
unsigned char gateway_ip[] = {
  192,168,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {
  255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {
  ""};		// max 32 bytes

unsigned char security_type = 3;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {
  ""};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = { 
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------


// This is our page serving function that generates web pages
boolean sendMyPage(char* URL) {
  if (strncmp(URL, "/print/",6) == 0) {

    char colorStr[6];
    memcpy(colorStr,URL+7,6);
    if (strncmp(colorStr,"random",6)==0){
      newColor=colors[rand()%6];
    }
    else{
      char charBuf[2];
      int ii,r,g,b;
      charBuf[0]=colorStr[0];
      charBuf[1]=colorStr[1];
      sscanf(charBuf, "%x", &r);
      charBuf[0]=colorStr[2];
      charBuf[1]=colorStr[3];
      sscanf(charBuf, "%x", &g);
      charBuf[0]=colorStr[4];
      charBuf[1]=colorStr[5];
      sscanf(charBuf, "%x", &b);
      newColor=matrix.Color(r, g, b);
    }
    matrix.setTextColor(newColor);

    strtok(URL,"=");
    message = "";
    message.concat(urlDecode(strtok(NULL,"=")));
    printedMessage=message;
    matrix.print(message);
    matrix.show();


    // Check if the requested URL matches "/"
    // Use WiServer's print and println functions to write out the page content
    WiServer.print(message);
    // URL was recognized
    return true;
  }
  // URL not found
  return false;
}

String urlDecode(String SRC) {
  String ret="";
  char ch;
  int i, ii;
  for (i=0; i<SRC.length(); i++) {
    if (int(SRC[i])==37) {
      char charBuf[10];
      SRC.substring(i+1,i+1+2).toCharArray(charBuf, 10) ;
      sscanf(charBuf, "%x", &ii);
      ch=static_cast<char>(ii);
      ret.concat(ch);
      i=i+2;
    } 
    else {
      ret.concat(SRC[i]);
    }
  }
  return ret;
}



void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  //matrix.set
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);

  // Initialize WiServer and have it use the sendMyPage function to serve pages

  // Enable Serial output and ask WiServer to generate log messages (optional)
  Serial.begin(57600);
  Serial.println("Starting");
  WiServer.init(sendMyPage);
  //WiServer.enableVerboseMode(true);
  Serial.println("Started");
}

int x    = matrix.width();
int pass = 0;


void loop(){

  // Run WiServer
  WiServer.server_task();
  Serial.println("Loop");

  matrix.fillScreen(0);
  matrix.setCursor(x, 0);

  int limit = -1*printedMessage.length()*7;
  //int limit = -120;
  //Serial.println(x);
  matrix.print(printedMessage);
  if(--x < limit) {
    x = matrix.width();
    //if(++pass >= 6) pass = 0;
    //matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(70);

  static unsigned long time = millis();
  if (millis()-time>10000){
    if (message!=""){
      printedMessage="";
      printedMessage.concat(message);
      message="";
      matrix.setTextColor(newColor);
    }    
    time=millis();
  }
}











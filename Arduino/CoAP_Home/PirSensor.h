#ifndef PIR__SENSOR
#define PIR__SENSOR

#include <CoapSensor.h>

class PirSensor : 
public CoapSensor {
public:
  int pin, status,outpin;
  long timestamp;

  PirSensor(char * name, int pin,int outpin) : 
  CoapSensor(name) {
    this->pin = pin;
    this->outpin=outpin;
    this->status = digitalRead(this->pin); // read the value from the sensor
    this->set_notify_time(30);
    this->timestamp=millis();
  }

  void get_value(uint8_t* output_data, size_t* output_data_len) {
    *output_data_len = sprintf((char*) output_data, "%d", this->status);
  }

  void check(void) {
    if (millis() - timestamp > 500) {
      int newStatus = digitalRead(this->pin); // read the value from the sensor
      if (newStatus != this->status) {
        this->changed = true;
      }
      if (this->status>0){
        digitalWrite(this->outpin,HIGH);
        timestamp = millis()+5000;
      }
      else{
        digitalWrite(this->outpin,LOW);
        timestamp = millis();
      }
      status = newStatus;
    }
  }
};

#endif



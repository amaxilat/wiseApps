#include <CoapSensor.h>

class TemperatureSensor : 
public CoapSensor {
public:
  int status;
  int pin;

  TemperatureSensor(char * name, int pin) : 
  CoapSensor(name) {
    //Temperature chip i/o
    this->pin=pin;
    this->status = getTemp();
    this->set_notify_time(300);
  }

  void get_value(uint8_t* output_data, size_t* output_data_len) {
    *output_data_len = sprintf((char*) output_data, "%d", this->status);
  }

  void check(void) {
    static unsigned long timestamp = 0;
    if (millis() - timestamp > 500) {
      int newStatus = getTemp();
      if (abs(newStatus -this->status) > 2) {
        this->changed = true;
      }
      status = newStatus;
      timestamp = millis();
    }
  }


  int getTemp(){
    //returns the temperature from one DS18S20 in DEG Celsius
#ifndef    FIO
    int TemperatureSum = ((((float)analogRead(this->pin))/1024.0*5000.0)-500.0)/10.0;
#else
    int TemperatureSum = ((((float)analogRead(this->pin))/1024.0*3300.0)-500.0)/10.0;
#endif
//    int TemperatureSum = map (analogRead(this->pin) , 0.4, 12 ,1,50) ;
    return TemperatureSum;
  }

};






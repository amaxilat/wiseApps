#ifndef LIGHT__SENSOR
#define LIGHT__SENSOR

#include <CoapSensor.h>

class LightSensor : public CoapSensor {
public:
    int pin;
    int status;

    LightSensor(char * name, int pin) : CoapSensor(name) {
        this->pin = pin;
        this->status = analogRead(this->pin); // read the value from the sensor
        this->set_notify_time(300);
    }

    void get_value(uint8_t* output_data, size_t* output_data_len) {
        *output_data_len = sprintf((char*) output_data, "%d", this->status);
    }

    void check(void) {
        static unsigned long timestamp = 0;
        if (millis() - timestamp > 3000) {
            int newStatus = 1024-analogRead(this->pin); // read the value from the sensor
            //newStatus = (newStatus/1024.0)*12000;
            if (((float)(newStatus -this->status))/this->status>0.1) {
                this->changed = true;
            }
            status = newStatus;
            timestamp = millis();
        }
    }
};

#endif

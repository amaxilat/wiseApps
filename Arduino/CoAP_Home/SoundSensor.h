#include "LightSensor.h"

/**
 * When connecting a 5K resistor before the Ground : volume=abs(analogValue-760);
 */
class SoundSensor : public LightSensor {
public:
    int pin, status;

    SoundSensor(char * name, int pin) : LightSensor(name,pin) {
      this->set_notify_time(300);
    }
};

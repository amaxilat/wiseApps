#include <coapSensor.h>


class parentSensor : 
public CoapSensor 
{
public:
  parentSensor(char * name):   
  CoapSensor(name)
  {
    this->set_notify_time(300);  
  }
  void set_parent(uint16_t parent){
    parent_=parent;
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
    *output_data_len = sprintf( (char*)output_data, "0x%x", parent_); 
  }
   void check(void)
  {

  }
  
private :
  uint16_t parent_;
};

/*
 * CTI Legacy AC Controller
 */

//TEMPERATURE Threshold
#define TARGET_TEMPERATURE 28

#define TARGET_MAC 0x2b0

#include "external_interface/external_interface.h"

#ifdef TARGET_TEMPERATURE
//ISENSE TEMPERATURE headers
#include <isense/modules/environment_module/environment_module.h>
#include <isense/modules/environment_module/temp_sensor.h>
#include <isense/modules/environment_module/light_sensor.h>
#endif


typedef wiselib::OSMODEL Os;

class HVACControllerApplication {
public:
    isense::EnvironmentModule* em_;

    void init(Os::AppMainParameter& value) {
        radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet(value);
        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);

        radio_->enable_radio();
        //channel switch
        radio_->set_channel(12);

        debug_->debug("AC controller Application boot");

#ifdef TARGET_TEMPERATURE
        //enable EM module
        em_ = new isense::EnvironmentModule(value);
        em_->enable(true);
        em_->temp_sensor()->enable();
#endif 
        //start timer
        timer_->set_timer<HVACControllerApplication, &HVACControllerApplication::start>(5000, this, 0);

    }
    // --------------------------------------------------------------------

    void start(void*) {
        Os::Radio::block_data_t message[6];

        message[0] = 0x7f;
        message[1] = 0x69;
        message[2] = 0x70;
        message[3] = 0x01;
        message[4] = 0xff;
        message[5] = 0x01;

#ifdef TARGET_TEMPERATURE
        //get temperature
        int8_t temp = em_->temp_sensor()->temperature();
        debug_->debug("Temperature Reading %d C", temp);

        if (temp <= TARGET_TEMPERATURE) {
            message[5] = 0x00;
        }
#endif

        radio_->send(TARGET_MAC, 6, message);
        // continue the periodic beaconing
        timer_->set_timer<HVACControllerApplication, &HVACControllerApplication::start>(30000, this, 0);
    }
private:
    Os::Radio::self_pointer_t radio_;
    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;
};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, HVACControllerApplication> app;
// --------------------------------------------------------------------------

void application_main(Os::AppMainParameter& value) {
    app.init(value);
}

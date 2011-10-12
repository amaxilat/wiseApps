#include <isense/application.h>
#include <isense/os.h>
#include <isense/dispatcher.h>
#include <isense/radio.h>
#include <isense/hardware_radio.h>
#include <isense/task.h>
#include <isense/timeout_handler.h>
#include <isense/isense.h>
#include <isense/uart.h>
#include <isense/time.h>
#include <isense/timer.h>
#include <isense/button_handler.h>
#include <isense/sleep_handler.h>
#include <isense/util/util.h>
#include <isense/modules/telosb_module/telosb_module.h>

#include <isense/platforms/msp430/msp430_os.h>
#include <isense/platforms/msp430/msp430_macros.h>

#include <io.h>
#include <signal.h>

//----------------------------------------------------------------------------
using namespace isense;

/**
 * When the Sensor Detects light lower than a threshold
 * A lamp turns on
 * and off when there is enough light
 */
class LightLampApplication :
public isense::Application,
public isense::Receiver,
public isense::Sender,
public isense::Task,
public isense::TimeoutHandler,
public isense::ButtonHandler {
public:
    LightLampApplication(isense::Os& os);

    ~LightLampApplication();

    ///From isense::Application
    void boot(void);

    ///From isense::ButtonHandler
    void button_down(uint8 button);

    ///From isense::ButtonHandler
    void button_up(uint8 button);

    ///From isense::Receiver
    void receive(uint8 len, const uint8 * buf, ISENSE_RADIO_ADDR_TYPE src_addr, ISENSE_RADIO_ADDR_TYPE dest_addr, uint16 signal_strength, uint16 signal_quality, uint8 seq_no, uint8 interface, Time time);

    ///From isense::Sender
    void confirm(uint8 state, uint8 tries, isense::Time time);

    ///From isense::Task
    void execute(void* userdata);

    ///From isense::TimeoutHandler
    void timeout(void* userdata);

    ///From isense::UartPacketHandler
    void handle_uart_packet(uint8 type, uint8* buf, uint8 length);

private:
    //application payloads
    uint8 messopen[13]; //on
    uint8 messclose[13]; //off
    uint8 messwitch[13]; //switch

    //echo payload
    uint8 echomsg [4];

    TelosbModule *telos;

    bool status;

    int16 light;

    bool is_gateway() {
        if (os().id() == 0xf042) {
            return true;
        } else {
            return false;
        }
    }



};

//----------------------------------------------------------------------------

LightLampApplication::
LightLampApplication(isense::Os& os)
: isense::Application(os),
telos(NULL),
status(false),
light(0) {
}

//----------------------------------------------------------------------------

LightLampApplication::
~LightLampApplication() {
}

//----------------------------------------------------------------------------

void
LightLampApplication::
boot(void) {
    os().debug("ON");

    TOGGLE_LED(LED_BLUE);
    os().allow_doze(false);
    os().allow_sleep(false);


    os().radio().hardware_radio().set_channel(17);

    telos = new TelosbModule(os_);

    telos->init();
    telos->add_button_handler(this);


    os().dispatcher().add_receiver(this);



    //Create the command payload - off
    messclose[0] = 0x7f; //mksense
    messclose[1] = 0x69; //mksense
    messclose[2] = 105; //mksense

    //Create the command payload - on
    messopen[0] = 0x7f; //mksense
    messopen[1] = 0x69; //mksense
    messopen[2] = 105; //mksense

    //Create the command payload - switch
    messwitch[0] = 0x7f; //mksense
    messwitch[1] = 0x69; //mksense
    messwitch[2] = 105; //mksense

    for (int i = 3; i < 13; i++) {
        messopen[i] = 1; //on
        messclose[i] = 0; //off
        messwitch[i] = 99; //swithc
    }

    //create the echo payload
    echomsg[0] = 0x7f; //mksense
    echomsg[1] = 0x69; //mksense
    echomsg[2] = 100; //mksense
    echomsg[3] = 42; //echo payload

    //get the initial light value
    light = telos->light();

    //start periodic tasks
    os().add_task_in(Time(1, 0), this, NULL);
}

//----------------------------------------------------------------------------

void
LightLampApplication::
button_down(uint8 button) {

    //button pressing changes status
    if (status) {
        os().radio().send(0xffff, 13, messopen, 0, 0);
        os().debug("on");
    } else {
        os().radio().send(0xffff, 13, messclose, 0, 0);
        os().debug("off");
    }
    status = !status;
}

void
LightLampApplication::
button_up(uint8 button) {

}

//----------------------------------------------------------------------------

void
LightLampApplication::
execute(void* userdata) {

    //grab new light value
    int16 newlight = telos->light();


    //check thresholds
    if (newlight < 200) {
        //need to switch off
        if (light >= 200) {
            // previously was on
            os().debug("Switch On The LAMP");
            os().radio().send(0xffff, 13, messopen, 0, 0);
            telos->led_on(1);
        }
    } else {
        //need to switch on
        if (light < 200) {
            //previously was off
            os().debug("Switch Off The LAMP");
            os().radio().send(0xffff, 13, messclose, 0, 0);
            telos->led_off(1);
        }
    }

    //set the light value as the current
    light = newlight;
    os().debug("light:%d", light);

    //broadcast a beacon
    os().radio().send(0xffff, 4, echomsg, 0, 0);
    os().debug("echo");

    //repeat every 1 minute
    os().add_task_in(Time(1, 0), this, NULL);
}

//----------------------------------------------------------------------------

void
LightLampApplication::
receive(uint8 len, const uint8* buf, ISENSE_RADIO_ADDR_TYPE src_addr, ISENSE_RADIO_ADDR_TYPE dest_addr, uint16 signal_strength, uint16 signal_quality, uint8 seq_no, uint8 interface, Time time) {

    //drop all
}

//----------------------------------------------------------------------------

void
LightLampApplication::
confirm(uint8 state, uint8 tries, isense::Time time) {

}

//----------------------------------------------------------------------------

void
LightLampApplication::
timeout(void* userdata) {

}

//----------------------------------------------------------------------------

/**
 */
isense::Application* application_factory(isense::Os& os) {
    return new LightLampApplication(os);
}

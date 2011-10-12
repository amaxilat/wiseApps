#include <isense/config.h>
#include <isense/os.h>
#include <isense/types.h>
#include <isense/application.h>
#include <isense/modules/security_module/lis_accelerometer.h>
#include <isense/util/ishell_interpreter.h>
#include <isense/data_handlers.h>


using namespace isense;
//--------------------------------------------------------------



class SecurityLampApplication :
public Application,
public BufferDataHandler,
public Receiver,
public Task {
public:
    // constructor
    SecurityLampApplication(Os &os);

    // inherited from application, called upon device boot
    void boot();

    // inherited from BufferDataHandler, called when
    // accelerometer data is available
    virtual void handle_buffer_data(BufferData* buf_data);

    // inherited from SensorHandler, called when a
    // passive infrared sensor event occurs
    virtual void handle_sensor();

    virtual void receive(isense::link_layer_length_t len, const uint8* msg, uint16 src_addr, uint16, uint16, uint16, uint8, uint8, isense::Time) {

        if (len == 4) {
            if ((msg[0] == 0x7f) && (msg[1] == 0x69) && (msg[2] == 100) && (msg[3] == 42)) {
                os().debug("received beacon from %x", src_addr);
            }
        }
    }

    void execute(void* userdata) {

        //send the beacon
        os().radio().send(0xffff, 4, echomsg, 0, 0);
        os().debug("echo");

        //if movement was detected
        if (moved) {

            os().radio().send(0xffff, 12, mess, 0, 0);
            os().debug("Accelerometer Send Event");

        }
        moved = false;

        //repeat every 1 second
        os().add_task_in(Time(1, 0), this, NULL);
    }


private:

    //ND beaconing
    uint8 echomsg [4];
    //movement payload
    uint8 mess[12];

    // pointer to the accelerometer
    LisAccelerometer* accelerometer_;
    bool moved;
};

//--------------------------------------------------------------

SecurityLampApplication::
SecurityLampApplication(Os &_os) :
Application(_os),
accelerometer_(NULL),
moved(false) {
}

//--------------------------------------------------------------

void
SecurityLampApplication::
boot() {

    os().debug("ON");

    //add receiver
    os().dispatcher().add_receiver(this);
    //change channel
    os().radio().hardware_radio().set_channel(17);

    // create LisAccelerometer instance
    accelerometer_ = new LisAccelerometer(os());


    if ((accelerometer_ != NULL)) {
        // ----- configure accelerometer ------
        // set threshold mode, i.e. the sensor will start to
        // deliver data after the set threshold was exceeded
        accelerometer_->set_mode(MODE_THRESHOLD);

        // set the threshold to 5 mg
        accelerometer_->set_threshold(180);

        // set this application as the sensor event handler
        // --> handle_buffer_data will be called if
        // sensor data is available
        accelerometer_->set_handler(this);

        // switch accelerometer
        accelerometer_->enable();
    } else
        os().fatal("Could not allocate accelerometer");



    //Create the beacon payload    
    echomsg[0] = 0x7f; //mksense 
    echomsg[1] = 0x69; //mksense 
    echomsg[2] = 100; //mksense     
    echomsg[3] = 42; //echo beacon


    //Create the command payload
    mess[0] = 0x7f; //mksense
    mess[1] = 0x69; //mksense
    mess[2] = 105; //mksense
    for (int i = 3; i < 12; i++) {
        mess[i] = 99; //movement payload
    }

    //start periodic tasks
    os().add_task_in(Time(1, 0), this, NULL);
}

//--------------------------------------------------------------

void
SecurityLampApplication::
handle_buffer_data(BufferData* buf_data) {
    //note the movement
    moved = true;

    //return from continuous mode to threshold mode
    accelerometer_->set_mode(MODE_THRESHOLD);
}

//--------------------------------------------------------------

void
SecurityLampApplication::
handle_sensor() {
}

//--------------------------------------------------------------

Application* application_factory(Os &os) {
    //create application instance
    return new SecurityLampApplication(os);
}



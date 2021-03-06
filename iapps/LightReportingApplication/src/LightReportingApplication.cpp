#include <isense/config.h>
#include <isense/os.h>
#include <isense/types.h>
#include <isense/application.h>
#include <isense/modules/environment_module/environment_module.h>
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
	if((minute+=15)==60){
	minute=0;
	hour++;
	if (hour==24){
	hour=0;
	day++;
	if (day==30){
	month++;
	}
	}
	}

	int time = minute*60+hour*60*60+day*24*60*60+month*30*24*60*60;
        uint32 lux = em_->light_sensor()->luminance();

        os().debug("%d-%d %d:%d",month, day,hour,minute);
        os().debug("%d$%d.0",time, lux);

        //repeat every 1 second
        os().add_task_in(Time(3, 0), this, NULL);
    }


private:
    // pointer to the accelerometer
    isense::EnvironmentModule* em_;
	int day;
	int month;
	int hour;
	int minute;
};

//--------------------------------------------------------------

SecurityLampApplication::
SecurityLampApplication(Os &_os) :
Application(_os),
em_(NULL),
day(0),
month(0),
hour(0),
minute(-30) {
}

//--------------------------------------------------------------

void
SecurityLampApplication::
boot() {

    os().debug("ON");

    //add receiver
    //    os().dispatcher().add_receiver(this);
    //change channel
    //    os().radio().hardware_radio().set_channel(17);

    // create LisAccelerometer instance
    em_ = new isense::EnvironmentModule(os());
    if (em_ != NULL) {
        em_->enable(true);
        if (em_->light_sensor()->enable()) {
//            em_->light_sensor()->set_data_handler(this);
            os().debug("em light");
        }
    }
    
        uint32 lux = em_->light_sensor()->luminance();
    //start periodic tasks
    execute(0);
}

//--------------------------------------------------------------

void
SecurityLampApplication::
handle_buffer_data(BufferData* buf_data) {

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



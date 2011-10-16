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
public Receiver,
public Task {
public:
    // constructor
    SecurityLampApplication(Os &os);

    // inherited from application, called upon device boot
    void boot();

    void debug_payload(const uint8 * payload, size_t length, uint16 src) {
        char buffer[1024];
        int bytes_written = 0;
        bytes_written += sprintf(buffer + bytes_written, "pl(%x)(", src);
        for (size_t i = 0; i < length; i++) {
            bytes_written += sprintf(buffer + bytes_written, "%x|", payload[i]);
        }
        bytes_written += sprintf(buffer + bytes_written, ")");
        buffer[bytes_written] = '\0';
        os().debug("%s", buffer);
    }

    virtual void receive(isense::link_layer_length_t len, const uint8* msg, uint16 src_addr, uint16, uint16, uint16, uint8, uint8, isense::Time) {
        debug_payload(msg, len, src_addr);
    }

    void execute(void* userdata) {
//
//        uint8 mess[1];
//        mess[0]=1;
//        os().radio().send(0xffff,1,mess,0,0);

	if (chan==26){
		chan=10;
	}
	//change channel
	os().radio().hardware_radio().set_channel(chan);
	os().debug("channel:%d",chan);

        os().debug("h");

        //repeat every 1 second
        os().add_task_in(Time(10, 0), this, NULL);
    }


private:
int chan;
};

//--------------------------------------------------------------

SecurityLampApplication::
SecurityLampApplication(Os &_os) :
Application(_os) {
}

//--------------------------------------------------------------

void
SecurityLampApplication::
boot() {

    os().debug("ON");

    os().allow_doze(false);
    os().allow_sleep(false);
    
    //add receiver
    os().dispatcher().add_receiver(this);
	chan=18;
	//change channel
	os().radio().hardware_radio().set_channel(chan);
	os().debug("channel:%d",chan);

    //start periodic tasks
    os().add_task_in(Time(10, 0), this, NULL);
}

//--------------------------------------------------------------

Application* application_factory(Os &os) {
    //create application instance
    return new SecurityLampApplication(os);
}



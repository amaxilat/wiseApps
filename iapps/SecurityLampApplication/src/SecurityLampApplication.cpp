/****************************************************
 *													*
 *  iSense Gateway Module Demo Application			*
 *  Version: 	1.1									*
 *  Date: 		11.01.2010							*
 *  Targets:	JN5139R1, JN5139R, JN5148			*
 * 	Author: 	Carsten Buschmann					*
 * 													*
 * 	Copyright 2006-2010								*
 * 	ALL RIGHTS RESERVED								*
 * 	coalesenses GmbH 								*
 * 	http://www.coalesenses.com						*
 * 													*
 ****************************************************/

#include <isense/config.h>
#include <isense/os.h>
#include <isense/types.h>
#include <isense/application.h>
#include <isense/modules/security_module/lis_accelerometer.h>
#include <isense/modules/security_module/pir_sensor.h>
#include <isense/util/ishell_interpreter.h>
#include <isense/data_handlers.h>


using namespace isense;


//--------------------------------------------------------------
class SecurityLampApplication :
	public Application,
	public BufferDataHandler,
	public SensorHandler,
	public Receiver,
	public Task
{
	public:
		// constructor
		SecurityLampApplication(Os &os);

		// inherited from application, called upon device boot
		void boot();

		// inherited from BufferDataHandler, called when
		// accelerometer data is available
		virtual void handle_buffer_data( BufferData* buf_data );

		// inherited from SensorHandler, called when a
		// passive infrared sensor event occurs
		virtual void handle_sensor();
		
		void receive(isense::link_layer_length_t len, const uint8* msg, uint16 src_addr, uint16, uint16, uint16, uint8, uint8, isense::Time){
			//os().debug("received from %x|%d",src_addr,len);
				if (len==4){
						if ((msg[0]==0x7f)&&(msg[1]==0x69)&&(msg[2]==100)&&(msg[3]==42)){
							os().debug("received echo from %x",src_addr);
						}
				}
		}
		
		
		void execute( void* userdata )	{
			uint8 echomsg [4] ;
			echomsg[0]=0x7f;
			echomsg[1]=0x69;
			echomsg[2]=100;
			echomsg[3]=42;
			os().radio().send(0xffff,4,echomsg,0,0);				
			os().debug("echo");	
			os().add_task_in(Time(1,0), this, NULL);
			
			
			
			if (moved){
				uint8 mess[12];
				mess[0]=0x7f;
				mess[1]=0x69;
				mess[2]=105;
				for (int i=3;i<12;i++){
					mess[i]=99;	
				}
				
				os().radio().send(0xffff,12,mess,0,0);
				os().debug("Accelerometer Send Event");
			
			}
			moved=false;
		}


	private:
	
		// pointer to the accelerometer
		LisAccelerometer* accelerometer_;
		bool moved;
};

//--------------------------------------------------------------
SecurityLampApplication::
SecurityLampApplication(Os &_os) :
	Application(_os),
	accelerometer_(NULL),
	moved(false)
{
}

//--------------------------------------------------------------
void
SecurityLampApplication::
boot ()
{
	// output boot notification message
	#if (ISENSE_RADIO_ADDRESS_LENGTH == 16)
		os().debug("Booting Security Module Demo Application, id=%x", os().id());
	#else
		os().debug("Booting Security Module Demo Application, id=%lx", os().id());
	#endif

	os().dispatcher().add_receiver(this);
	os().radio().hardware_radio().set_channel(17);
	
	os().add_task_in(Time(1,0), this, NULL);
	
	// create LisAccelerometer instance
	accelerometer_ = new LisAccelerometer(os());
	

	if ((accelerometer_ != NULL))
	{
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
		os().fatal("Could not allocate sensors/iShell interpreter");
}

//--------------------------------------------------------------
void
SecurityLampApplication::
handle_buffer_data( BufferData* buf_data )
{
	moved=true;
	
	//return from continuous mode to threshold mode
	accelerometer_->set_mode(MODE_THRESHOLD);
}

//--------------------------------------------------------------
void
SecurityLampApplication::
handle_sensor()
{	
}

//--------------------------------------------------------------
Application* application_factory(Os &os)
{
	//create application instance
	return new SecurityLampApplication(os);
}



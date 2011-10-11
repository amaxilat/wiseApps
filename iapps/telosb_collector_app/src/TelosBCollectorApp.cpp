/************************************************************************
 ** This file is part of the the iSense project.
 ** Copyright (C) 2006 coalesenses GmbH (http://www.coalesenses.com)
 ** ALL RIGHTS RESERVED.
 ************************************************************************/
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
/**
 */

using namespace isense;

class iSenseDemoApplication :
	public isense::Application,
	public isense::Receiver,
	public isense::Sender,
	public isense::Task,
	public isense::TimeoutHandler,
	public isense::ButtonHandler,
	public isense::UartPacketHandler
{
public:
	iSenseDemoApplication(isense::Os& os);

	~iSenseDemoApplication() ;

	///From isense::Application
	void boot (void) ;

	///From isense::ButtonHandler
	void button_down( uint8 button );

	///From isense::ButtonHandler
	void button_up(uint8 button);

	///From isense::Receiver
	void receive (uint8 len, const uint8 * buf, ISENSE_RADIO_ADDR_TYPE src_addr, ISENSE_RADIO_ADDR_TYPE dest_addr, uint16 signal_strength, uint16 signal_quality, uint8 seq_no, uint8 interface, Time time) ;

	///From isense::Sender
	void confirm (uint8 state, uint8 tries, isense::Time time) ;

	///From isense::Task
	void execute( void* userdata ) ;

	///From isense::TimeoutHandler
	void timeout( void* userdata ) ;

	///From isense::UartPacketHandler
	void handle_uart_packet( uint8 type, uint8* buf, uint8 length );

private:
	uint8 messopen[13];
	uint8 messclose[13];
	uint8 messwitch[13];
	TelosbModule *telos;
	int counter;
	bool status;
	
	int16 light;

	bool is_gateway(){
		if (os().id()==0xf042){
		return true;
		}
		else {
		return false;
		}
	}



};

//----------------------------------------------------------------------------
iSenseDemoApplication::
	iSenseDemoApplication(isense::Os& os)
	: isense::Application(os),
		telos(NULL),
		status(false),
		light(0)
	{
	}

//----------------------------------------------------------------------------
iSenseDemoApplication::
	~iSenseDemoApplication()

	{
	}

//----------------------------------------------------------------------------
void
	iSenseDemoApplication::
	handle_uart_packet(uint8 type, uint8 * buf, uint8 length)
{
	os().debug("uart");
}

//----------------------------------------------------------------------------
void
	iSenseDemoApplication::
	boot(void)
	{
        os().debug("App::boot ");
        os().debug("App::boot %x",os().id());
		TOGGLE_LED(LED_BLUE);
        os().allow_doze(false);
        os().allow_sleep(false);


        telos = new TelosbModule(os_);

        telos->init();
        telos->add_button_handler(this);

        os().add_task_in(Time(1,0), this, NULL);

        os().dispatcher().add_receiver(this);
        
        
        	
	messclose[0]=0x7f;
	messclose[1]=0x69;
	messclose[2]=105;
	
	messopen[0]=0x7f;
	messopen[1]=0x69;
	messopen[2]=105;
	
	messwitch[0]=0x7f;
	messwitch[1]=0x69;
	messwitch[2]=105;
		
	for (int i=3;i<13;i++){
		messopen[i]=1;	
		messclose[i]=0;
	}

	light = telos->light();


	os().radio().hardware_radio().set_channel(17);
	counter=0;

 	}

//----------------------------------------------------------------------------

void
	iSenseDemoApplication::
	button_down( uint8 button )
	{
			
			
			if (status){
			os().radio().send(0xffff,13,messopen,0,0);
			os().debug("on");
		}else{
			os().radio().send(0xffff,13,messclose,0,0);			
			os().debug("off");
			}
			status=!status;
	
	
		//telos->led_on(0);
	}

void
	iSenseDemoApplication::
	button_up( uint8 button )
	{
		//telos->led_off(0);
	}

//----------------------------------------------------------------------------
void
	iSenseDemoApplication::
	execute( void* userdata )
	{
	int16 newlight = telos->light();
	
	if (newlight<200){
			if (light>=200){
				os().debug("Switch On The LAMP");
				os().radio().send(0xffff,13,messopen,0,0);
				telos->led_on(1);
			}
	}else {
			if (light<200){
			os().debug("Switch Off The LAMP");
			os().radio().send(0xffff,13,messclose,0,0);			
			telos->led_off(1);
		}
	}
	
	light=newlight;
	os().debug("light:%d",light);
	

	
	uint8 echomsg [4] ;
	echomsg[0]=0x7f;
	echomsg[1]=0x69;
	echomsg[2]=100;
	echomsg[3]=42;
	os().radio().send(0xffff,4,echomsg,0,0);				
	os().debug("echo");
	
	os().add_task_in(Time(1,0), this, NULL);
	}

//----------------------------------------------------------------------------
void
	iSenseDemoApplication::
	receive (uint8 len, const uint8* buf, ISENSE_RADIO_ADDR_TYPE src_addr, ISENSE_RADIO_ADDR_TYPE dest_addr, uint16 signal_strength, uint16 signal_quality, uint8 seq_no, uint8 interface, Time time)
	{
	
	}

//----------------------------------------------------------------------------
void
	iSenseDemoApplication::
	confirm (uint8 state, uint8 tries, isense::Time time)
	{
		
	}

//----------------------------------------------------------------------------
void
	iSenseDemoApplication::
	timeout( void* userdata )
	{
		
	}

//----------------------------------------------------------------------------
/**
  */
isense::Application* application_factory(isense::Os& os)
{
	return new iSenseDemoApplication(os);
}


/*-----------------------------------------------------------------------
* Source  $Source: $
* Version $Revision: 1.24 $
* Date    $Date: 2006/10/19 12:37:49 $
*-----------------------------------------------------------------------
* $Log$
*-----------------------------------------------------------------------*/

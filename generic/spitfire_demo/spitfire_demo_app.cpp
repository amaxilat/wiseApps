/**
 * SpitFire Demo Application using the Integer based Semantic Storage
 */

#include "external_interface/external_interface.h"
#define USE_SENSORS
#ifdef USE_SENSORS
#include <isense/modules/environment_module/environment_module.h>
#include <isense/modules/security_module/pir_sensor.h>


#ifdef SHAWN

#endif
#ifdef ISENSE
#include "external_interface/isense/isense_light_sensor.h"
#endif
#endif

#include "algorithms/cluster/clustering_types.h"

#define VISUALIZER
#define ENABLE_UART_CL

typedef wiselib::OSMODEL Os;


//#define VIRTUAL_RADIO
#ifdef VIRTUAL_RADIO
#include "util/wisebed_node_api/virtual_extended_txradio.h"
#include "util/base_classes/routing_base.h"
typedef Os::TxRadio hRadio;
typedef wiselib::VirtualExtendedTxRadioModel<Os, Os::TxRadio, Os::Uart> Radio;
#else 
typedef Os::TxRadio Radio;
#endif

typedef Os::Radio::node_id_t node_id_t;
typedef Os::Radio::block_data_t block_data_t;

#include "algorithms/cluster/semantics.h"
typedef wiselib::Semantics<Os> semantics_t;

// Replace the first Algorithm name with one from the list in comment
#define SPIT
#ifdef SPIT
#include "algorithms/cluster/spit/spit_core.h"
#endif


#ifdef SPIT

typedef wiselib::SemanticClusterHeadDecision<Os, Radio, semantics_t> CHD_t;
typedef wiselib::SemanticJoinDecision<Os, Radio, semantics_t> JD_t;
typedef wiselib::SemaIterator<Os, Radio, semantics_t> IT_t;
typedef wiselib::SpitCore<Os, Radio, CHD_t, JD_t, IT_t, semantics_t> clustering_algo_t;
#endif

typedef Os::Uart::size_t uart_size_t;

class ClusteringFronts :
public isense::Uint32DataHandler,
public isense::Int8DataHandler
//,public isense::SensorHandler 
{
public:

    void handle_uint32_data(uint32 value) {
    }

    //--------------------------------------------------------------

    void handle_int8_data(int8 value) {
    }

    void init(Os::AppMainParameter& value) {
        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);
        debug_->debug("*B*");
        clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet(value);
#ifdef ENABLE_UART_CL
        uart_ = &wiselib::FacetProvider<Os, Os::Uart>::get_facet(value);
#endif
        rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet(value);


#ifdef VIRTUAL_RADIO
        radio_ = &virtual_radio_;
        hardware_radio_ = &wiselib::FacetProvider<Os, Os::TxRadio>::get_facet(value);
        virtual_radio_.init(*hardware_radio_, *uart_, *debug_);

#else 
        radio_ = &wiselib::FacetProvider<Os, Os::TxRadio>::get_facet(value);
#endif

        radio_->enable_radio();

        //        routing_.init(*radio_, *debug_);
        //        routing_.enable_radio();


        rand_->srand(radio_->id());

        debug_->debug("em");
        em_ = new isense::EnvironmentModule(value);
        if (em_ != NULL) {
            if (em_->light_sensor() != NULL) {
                em_->light_sensor()->set_data_handler(this);
                //os().add_task_in(Time(10, 0), this, (void*) TASK_SET_LIGHT_THRESHOLD);
            } else {
                //            os().debug("iSense::%x Could not allocate light sensor", os().id());
            }
            if (em_->temp_sensor() != NULL) {
                em_->temp_sensor()->set_data_handler(this);
            } else {
                //            os().debug("iSense::%x Could not allocate temp sensor", os().id());
            }


            //        os().debug("iSense::%x::enabled em", os().id());
            em_->enable(true);
        }


            if (em_->light_sensor()->enable() == true ){
                debug_->debug("Light Sensor is present");
            }else{
                debug_->debug("No Light Sensor here");
            }

        //        debug_->debug("pir");
                pir_ = new isense::PirSensor(value);
		if (pir_->enable()){
	                debug_->debug("Pir Sensor is present");
        	    }else{
                	debug_->debug("No Pir Sensor here");
		}
        //        // ----- configure PIR sensor -------------
        //        // set this application as the sensor event handler
        //        // --> handle_sensor will be called upon a PIR event
        //        pir_->set_sensor_handler(this);
        //        //set the PIR event duration to 2 secs
        //        pir_->set_pir_sensor_int_interval(2000);
        //        // switch on the PIR sensor
        //        pir_->enable();


#ifdef CHANGE_POWER
        TxPower power;
        power.set_dB(DB);
        radio_->set_power(power);
#endif

        //radio_->set_channel(18);

#ifdef ENABLE_UART_CL
        uart_->reg_read_callback<ClusteringFronts, &ClusteringFronts::handle_uart_msg > (this);
        uart_->enable_serial_comm();
#endif



        timer_->set_timer<ClusteringFronts, &ClusteringFronts::start > (1000, this, 0);

    }

    void handle_sensor() {
        semantics_.set_semantic_value(semantics_t::PIR, 1);
    }

    bool is_otap() {
        return false;
    }

    // --------------------------------------------------------------------

    void start(void * a) {
        if (a == 0) {
            disabled_ = false;
            // set the HeadDecision Module
            clustering_algo_.set_cluster_head_decision(CHD_);
            // set the JoinDecision Module
            clustering_algo_.set_join_decision(JD_);
            // set the Iterator Module
            clustering_algo_.set_iterator(IT_);
            clustering_algo_.init(*radio_, *timer_, *debug_, *rand_, semantics_);

            //debug_->debug("ON");
            disabled_ = true;
            //            neighbor_discovery.enable();
            //clustering_algo_.enable(40);

#ifdef VISUALIZER
            if (!is_otap()) {
                //neighbor_discovery.register_debug_callback(0);
                clustering_algo_.register_debug_callback();
            }
#endif

        }

        //        semantics_.set_semantic_value(semantics_t::PIR, 0);
        semantics_.set_semantic_value(semantics_t::LIGHT, em_->light_sensor()->luminance());
        semantics_.set_semantic_value(semantics_t::TEMP, em_->temp_sensor()->temperature());

        timer_->set_timer<ClusteringFronts,
                &ClusteringFronts::start > (30000, this, (void *) 1);

    }

#ifdef ENABLE_UART_CL

    void handle_uart_msg(uart_size_t len, Os::Uart::block_data_t *data) {
        //debug_->debug("Got a uart message %x", radio_->id());

        if (data[0] == 0x2) {
            //if an enable message
            if (!is_otap()) {
                switch (data[1]) {
                    case 0x1:
                        enable();
                        break;
                    case 0x2:
                        disable();
                        break;
                    case 0x6:
                        //set semantic - integer mode
                        int semantic_id, semantic_value;
                        memcpy(&semantic_id, data + 2, sizeof (int));
                        memcpy(&semantic_value, data + 6, sizeof (int));
                        set_semantic(semantic_id, semantic_value);
                        //command.set_controll_type(ControllMsg_t::CHANGE_K);
                        //command.set_payload(data[2]);
                        break;
                    case 0x7:

                        clustering_algo_.reset_demands();
                        //run a query
                        for (uint8_t pos = 2; pos < len; pos += 2 * sizeof (int)) {
                            int semantic_id, semantic_value;
                            memcpy(&semantic_id, data + pos, sizeof (int));
                            memcpy(&semantic_value, data + pos + 4, sizeof (int));
                            set_demands(semantic_id, semantic_value);
                        }
                        query(0);
                        break;
                    case 0x8:
                        clustering_algo_.reset_demands();
                        //run a query
                        for (uint8_t pos = 2; pos < len; pos += 1 * sizeof (int)) {
                            int semantic_id, semantic_value;
                            memcpy(&semantic_id, data + pos, sizeof (int));
                            semantic_value = 1;
                            set_demands(semantic_id, semantic_value);
                        }
                        query(1);
                        break;

                }
            }
        }
    }
#endif


private:

    void enable() {
        if (disabled_) {
            debug_->debug("ON");
            clustering_algo_.enable(5);
            disabled_ = false;
        }
    }

    void disable() {
        if (!disabled_) {
            debug_->debug("OFF");
            clustering_algo_.disable();
            disabled_ = true;
        }
    }

    void set_semantic(int id, int value) {
        debug_->debug("SS;%d;%d", id, value);
        semantics_.set_semantic_value(id, value);
    }

    void set_demands(int id, int value) {
        debug_->debug("SD;%d;%d", id, value);
        clustering_algo_.set_demands(id, value);

        //        semantics_.set_semantic_value(semantics_t::PIR, 0);
        semantics_.set_semantic_value(semantics_t::LIGHT, em_->light_sensor()->luminance());
        semantics_.set_semantic_value(semantics_t::TEMP, em_->temp_sensor()->temperature());
    }

    void query(int i) {
        clustering_algo_.answer((void *) i);
    }
    //    routing_t routing_;

    bool clustering_enabled_;

    bool head_dropped_;
    uint16_t period;
    int counter;

#ifdef VIRTUAL_RADIO
    Radio virtual_radio_;
    Radio * radio_;
    Os::TxRadio::self_pointer_t hardware_radio_;
#else 
    Os::TxRadio::self_pointer_t radio_;
#endif

    // clustering algorithm modules
    CHD_t CHD_;
    JD_t JD_;
    IT_t IT_;
    // clustering algorithm core component
    clustering_algo_t clustering_algo_;
    bool disabled_;

    semantics_t semantics_;


    isense::EnvironmentModule* em_;
        isense::PirSensor* pir_;


    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;
    Os::Clock::self_pointer_t clock_;
#ifdef ENABLE_UART_CL
    Os::Uart::self_pointer_t uart_;
#endif
    Os::Rand::self_pointer_t rand_;
};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ClusteringFronts> example_app;
// --------------------------------------------------------------------------

void application_main(Os::AppMainParameter& value) {
    example_app.init(value);
}

/**
 * Clustering Application Using Echo
 */

#include "external_interface/external_interface.h"

#ifdef ISENSE
#define USE_SENSORS

#endif

#ifdef USE_SENSORS
#include <isense/modules/environment_module/environment_module.h>
#include <isense/modules/security_module/pir_sensor.h>
#endif

#include "algorithms/cluster/clustering_types.h"

//#define ROUTING
#ifdef ROUTING
#include "internal_interface/routing_table/routing_table_static_array.h"
#include "algorithms/routing/flooding/flooding_algorithm.h"
#endif

#define VISUALIZER
#define ENABLE_UART_CL

typedef wiselib::OSMODEL Os;
typedef Os::Radio::node_id_t node_id_t;
typedef Os::block_data_t block_data_t;

//#define VIRTUAL_RADIO
#ifdef VIRTUAL_RADIO
#include "util/wisebed_node_api/virtual_extended_txradio.h"
#include "util/base_classes/routing_base.h"
typedef Os::TxRadio hRadio;
typedef wiselib::VirtualExtendedTxRadioModel<Os, Os::TxRadio, Os::Uart> Radio;
#else 
typedef Os::TxRadio Radio;
#endif

#include "algorithms/neighbor_discovery/echo.h"

typedef wiselib::Echo<Os, Radio, Os::Timer, Os::Debug> nb_t;

#define INTEGER_STORAGE
#ifdef INTEGER_STORAGE
#include "algorithms/cluster/semantics.h" 
typedef wiselib::Semantics<Os> semantics_t;
#else
#include "util/allocators/malloc_free_allocator.h"
#include "algorithms/cluster/spit/tuple_store_adaptor.h"
typedef wiselib::MallocFreeAllocator<Os> allocator_t;
typedef wiselib::TupleStoreAdaptor<Os, allocator_t> semantics_t;
#endif



#ifdef ROUTING
typedef wiselib::StaticArrayRoutingTable<Os, Os::Radio, 64 > FloodingStaticMap;
typedef wiselib::FloodingAlgorithm<Os, FloodingStaticMap, Os::TxRadio, Os::Debug> routing_t;
#endif



// Replace the first Algorithm name with one from the list in comment
#define GROUP
#ifdef GROUP
#include "algorithms/cluster/group/group_core.h"
#endif


#ifdef GROUP
typedef wiselib::NothingClusterHeadDecision<Os, Radio, semantics_t> CHD_t;
typedef wiselib::GroupJoinDecision<Os, Radio, semantics_t> JD_t;
typedef wiselib::GroupIterator<Os, Radio, semantics_t> IT_t;
typedef wiselib::GroupCore<Os, Radio, CHD_t, JD_t, IT_t, nb_t, semantics_t> clustering_algo_t;
#endif

typedef Os::Uart::size_t uart_size_t;

typedef semantics_t::predicate_t predicate_t;
typedef semantics_t::value_t value_t;

class SemanticGroupsApp
#ifdef ISENSE
:
public isense::Uint32DataHandler
,public isense::Int8DataHandler
,public isense::SensorHandler
#endif
{
public:

    void handle_uint32_data(uint32_t value) {
    }

    //--------------------------------------------------------------

    void handle_int8_data(int8_t value) {
    }

    void init(Os::AppMainParameter& value) {

        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);
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
        debug_->debug("*Boot*%x", radio_->id());

        radio_->enable_radio();
        //        routing_.init(*radio_, *debug_);
        //        routing_.enable_radio();    
        rand_->srand(radio_->id());

#ifdef ISENSE
        em_ = new isense::EnvironmentModule(value);
        if (em_ != NULL) {
            if (em_->light_sensor()->enable()) {

                light_sensor_ = true;
                em_->light_sensor()->set_data_handler(this);
                //os().add_task_in(Time(10, 0), this, (void*) TASK_SET_LIGHT_THRESHOLD);
            } else {
                //            os().debug("iSense::%x Could not allocate light sensor", os().id());
            }
            if (em_->temp_sensor()->enable()) {

                temp_sensor_ = true;
                em_->temp_sensor()->set_data_handler(this);
            } else {
                //            os().debug("iSense::%x Could not allocate temp sensor", os().id());
            }

            //        os().debug("iSense::%x::enabled em", os().id());
            em_->enable(true);

        }

        pir_ = new isense::PirSensor(value);
        if (pir_->enable()) {
            pir_sensor_ = true;

            // ----- configure PIR sensor -------------
            // set this application as the sensor event handler
            // --> handle_sensor will be called upon a PIR event
            pir_->set_sensor_handler(this);
            //set the PIR event duration to 2 secs
            pir_->set_pir_sensor_int_interval(2000);

        }
#endif

        //        radio_->set_channel(17);

#ifdef ENABLE_UART_CL
        uart_->reg_read_callback<SemanticGroupsApp, &SemanticGroupsApp::handle_uart_msg > (this);
        uart_->enable_serial_comm();
#endif        
        timer_->set_timer<SemanticGroupsApp, &SemanticGroupsApp::start > (1000, this, 0);

#ifdef ISENSE
        uint8_t echo_threshold = 200;
        if ((radio_->id() >= 400) && (radio_->id() < 431)) { //TUBSnodes

            Radio::TxPower power;
            power.set_dB(-12);
            radio_->set_power(power);

            echo_threshold = 180;


        } else if ((radio_->id() >= 0x2000) && (radio_->id() < 0x2200)) {//UZLnodes

            Radio::TxPower power;
            power.set_dB(-12);
            radio_->set_power(power);
            debug_->debug("power set to -12");
            echo_threshold = 180;
        } else {

            switch (radio_->id()) {
                case 0x9710: case 0x1b7f: case 0x99ad: case 0x96e5: case 0x999d: case 0x1b95: case 0x15d3: case 0x9731: case 0x96c2: case 0x9700: case 0x1725: case 0x96fc: case 0x96f4: case 0x99bd: case 0x15e0: case 0x1b3b: case 0x1520: case 0x1b4c: case 0x1cb9: case 0x1b84: case 0x9708: case 0x96e0: case 0x9a0d: case 0x1b99: case 0x96eb: case 0x1be5: case 0x9a0c:
                    //UNIGEnodes

                    Radio::TxPower power;
                    power.set_dB(-12);
                    radio_->set_power(power);

                    echo_threshold = 180;
                    break;
            }
        }
        neighbor_discovery.init(*radio_, *clock_, *timer_, *debug_, 2500, 40000, echo_threshold, 255);
#else    

#endif
        neighbor_discovery.register_debug_callback(nb_t::NEW_NB | nb_t::NEW_NB_BIDI | nb_t::DROPPED_NB | nb_t::LOST_NB_BIDI);
        neighbor_discovery.enable();
    }

    void handle_sensor() {
        //        debug_->debug("pir event from node %x", radio_->id());
        if (pir_sensor_) {
#ifdef INTEGER_STORAGE
            int pir = semantics_t::PIR, value = 1;
            semantics_.set_semantic_value(predicate_t((block_data_t*) & pir, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
            debug_->debug("Sread:%d:%d", pir, value);
#endif
        }
    }

    bool is_otap() {
        return false;
    }

    // --------------------------------------------------------------------

    void start(void * a) {

        if (a == 0) {

            disabled_ = false;
            //            neighbor_discovery.init(*radio_, *clock_, *timer_, *debug_, 1000, 10000, 200, 230);
            // set the HeadDecision Module
            clustering_algo_.set_cluster_head_decision(CHD_);
            // set the JoinDecision Module
            clustering_algo_.set_join_decision(JD_);
            // set the Iterator Module
            clustering_algo_.set_iterator(IT_);
            clustering_algo_.init(*radio_, *timer_, *debug_, *rand_, neighbor_discovery, semantics_);


            disabled_ = true;

#ifndef ISENSE
            enable();
            int p = 2;
            int val = 1;
            semantics_.set_semantic_value(predicate_t((block_data_t*) & p), value_t((block_data_t*) & val));


            p = 211;
            val = rand()(35);
            semantics_.set_semantic_value(predicate_t((block_data_t*) & p), value_t((block_data_t*) & val));
            p = 210;
            val = rand()(250);
            semantics_.set_semantic_value(predicate_t((block_data_t*) & p), value_t((block_data_t*) & val));

            p = 4;
            val = 3;
            semantics_.set_semantic_value(predicate_t((block_data_t*) & p), value_t((block_data_t*) & val));

#endif 


            clustering_algo_.register_debug_callback();

        }

#ifdef ISENSE //update semantics part
#ifdef INTEGER_STORAGE

        if (pir_sensor_) {
            int pir = semantics_t::PIR, value = 0;
            semantics_.set_semantic_value(predicate_t((block_data_t*) & pir, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
            debug_->debug("Sread:%d:%d", pir, value);
        }
        if (light_sensor_) {

            int light = semantics_t::LIGHT, value = em_->light_sensor()->luminance();
            if ((value < 4000) && (value > 0)) {
                semantics_.set_semantic_value(predicate_t((block_data_t*) & light, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
                debug_->debug("Sread:%d:%d", light, value);
            }
        }
        if (temp_sensor_) {

            int temp = semantics_t::TEMP, value = em_->temp_sensor()->temperature();
            if ((value < 4000) && (value > 0)) {
                semantics_.set_semantic_value(predicate_t((block_data_t*) & temp, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
                debug_->debug("Sread:%d:%d", temp, value);
            }
        }
#endif
#endif

        timer_->set_timer<SemanticGroupsApp,
                &SemanticGroupsApp::start > (60000, this, (void *) 1);

    }

#ifdef ENABLE_UART_CL

    void handle_uart_msg(uart_size_t len, Os::Uart::block_data_t *data) {
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
                        set_semantic((block_data_t*) data + 2, (block_data_t*) data + 6);
                        //command.set_controll_type(ControllMsg_t::CHANGE_K);
                        //command.set_payload(data[2]);
                        break;
                    case 0x7:
                        //                        clustering_algo_.reset_demands();
                        //run a query
                        //                        for (uint8_t pos = 2; pos < len; pos += 2 * sizeof (int)) {
                        //                            int semantic_id, semantic_value;
                        //                            memcpy(&semantic_id, data + pos, sizeof (int));
                        //                            memcpy(&semantic_value, data + pos + 4, sizeof (int));
                        //                            set_demands(semantic_id, semantic_value);
                        //                        }
                        //                        query(0);
                        break;
                    case 0x8:
                        //                        clustering_algo_.reset_demands();
                        //run a query
                        //                        for (uint8_t pos = 2; pos < len; pos += 1 * sizeof (int)) {
                        //                            int semantic_id, semantic_value;
                        //                            memcpy(&semantic_id, data + pos, sizeof (int));
                        //                            semantic_value = 1;
                        //                            set_demands(semantic_id, semantic_value);
                        //                        }
                        //                        query(1);
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

            clustering_algo_.enable();
            disabled_ = false;
        }
    }

    void disable() {
        if (!disabled_) {
            debug_->debug("OFF");
            //            neighbor_discovery.disable();
            clustering_algo_.disable();
            disabled_ = true;
        }
    }

    //    void change_k(uint8_t k) {
    //        debug_->debug("ChangeK;%x;%d", radio_->id(), k);
    //        //        clustering_algo_.set_maxhops(k);
    //    }

    void set_semantic(block_data_t* id, block_data_t* value) {
        predicate_t pred = predicate_t(id);
        value_t val = value_t(value);
        debug_->debug("SS;%s;%s", pred.c_str(), val.c_str());
        semantics_.set_semantic_value(pred, val);
    }
    //
    //    void set_demands(int id, int value) {
    //        debug_->debug("SD;%d;%d", id, value);
    //        //        clustering_algo_.set_demands(id, value);
    //    }
    //
    //    void query(int i) {
    //        //        clustering_algo_.answer(i);
    //    }

    //    routing_t routing_;

    bool clustering_enabled_;

#ifdef VIRTUAL_RADIO
    Radio virtual_radio_;
    Radio * radio_;
    Os::TxRadio::self_pointer_t hardware_radio_;
#else 
    Os::TxRadio::self_pointer_t radio_;
#endif

#ifdef REMOTE_DEBUG
    remote_debug_t remote_debug_;
    flooding_t flooding_;
    routing_t routing_;
#endif

    nb_t neighbor_discovery;
    // clustering algorithm modules
    CHD_t CHD_;
    JD_t JD_;
    IT_t IT_;
    // clustering algorithm core component
    clustering_algo_t clustering_algo_;
    bool disabled_;

    semantics_t semantics_;

    bool light_sensor_, temp_sensor_, pir_sensor_;

#ifdef ISENSE
    isense::EnvironmentModule* em_;
    isense::PirSensor* pir_;
#endif

    Os::Rand & rand() {
        return *rand_;
    }

    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;
    Os::Clock::self_pointer_t clock_;
#ifdef ENABLE_UART_CL
    Os::Uart::self_pointer_t uart_;
#endif
    Os::Rand::self_pointer_t rand_;
};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, SemanticGroupsApp> example_app;
// --------------------------------------------------------------------------

void application_main(Os::AppMainParameter& value) {
    example_app.init(value);
}

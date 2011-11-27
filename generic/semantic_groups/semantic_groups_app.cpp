/**
 * Clustering Application Using Echo
 */

#include "external_interface/external_interface.h"

#ifdef ISENSE
//#define USE_SENSORS
#define CHANGE_POWER
#define CHANGE_CHANNEL
#endif

#ifdef USE_SENSORS
#include <isense/modules/environment_module/environment_module.h>
//#include <isense/modules/security_module/pir_sensor.h>
#endif

#include "algorithms/cluster/clustering_types.h"

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
typedef Os::Uart::size_t uart_size_t;

//INCLUDE ALGORITHMS

//Neighbor Discovery
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

//SE formation
#include "algorithms/cluster/group/group_core.h"
typedef wiselib::NothingClusterHeadDecision<Os, Radio, semantics_t> CHD_t;
typedef wiselib::GroupJoinDecision<Os, Radio, semantics_t> JD_t;
typedef wiselib::GroupIterator<Os, Radio, semantics_t> IT_t;
typedef wiselib::GroupCore<Os, Radio, CHD_t, JD_t, IT_t, nb_t, semantics_t> clustering_algo_t;

//SE Quering
#include "algorithms/routing/query/queryMsg.h"
typedef wiselib::QueryMsg<Os, Radio> QueryMsg_t;

#include "algorithms/routing/query/se_query_rounting.h"
typedef wiselib::SeQueryRouting<Os, Radio, Os::Timer, semantics_t, nb_t> seQueryRouting_t;

//#include "algorithms/routing/query/se_query_flooding.h"
//typedef wiselib::SeQueryFlooding<Os, Radio, Os::Timer, semantics_t, nb_t> seQueryRouting_t;

#ifdef USE_RELIABLE
#include "radio/reliable/reliableradio.h"
typedef wiselib::ReliableRadio<Os, Radio, Os::Timer, Os::Debug> rel_radio_t;
#endif

class SemanticGroupsApp
#ifdef ISENSE
#ifdef USE_SENSORS
:
public isense::Uint32DataHandler
, public isense::Int8DataHandler
//, public isense::SensorHandler
#endif
#endif
{
public:

    typedef semantics_t::predicate_t predicate_t;
    typedef semantics_t::value_t value_t;

    void handle_uint32_data(uint32_t value) {
    }

    //--------------------------------------------------------------

    void handle_int8_data(int8_t value) {
    }

    void init(Os::AppMainParameter& value) {

        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);
        debug().debug("*Boot*%x", radio_->id());

        clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet(value);
        uart_ = &wiselib::FacetProvider<Os, Os::Uart>::get_facet(value);
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

#ifdef ISENSE
#ifdef USE_SENSORS
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

        //        pir_ = new isense::PirSensor(value);
        //        if (pir_->enable()) {
        //            pir_sensor_ = true;
        //
        //            // ----- configure PIR sensor -------------
        //            // set this application as the sensor event handler
        //            // --> handle_sensor will be called upon a PIR event
        //            pir_->set_sensor_handler(this);
        //            //set the PIR event duration to 2 secs
        //            pir_->set_pir_sensor_int_interval(2000);
        //
        //        }
#endif
#endif

#ifdef CHANGE_CHANNEL
        radio_->set_channel(16);
#endif

        uart_->reg_read_callback<SemanticGroupsApp, &SemanticGroupsApp::handle_uart_msg > (this);
        uart_->enable_serial_comm();

        timer_->set_timer<SemanticGroupsApp, &SemanticGroupsApp::start > (1000, this, 0);

#ifdef ISENSE
        uint8_t echo_threshold = 190;
        if ((radio_->id() >= 0x2000) && (radio_->id() < 0x2200)) {//UZLnodes

#ifdef CHANGE_POWER
            Radio::TxPower power;
            power.set_dB(0);
            radio_->set_power(power);
            debug().debug("power set to 0");
#endif

            echo_threshold = 190;
        } else if ((radio_->id() >= 400) && (radio_->id() < 431)) { //TUBSnodes

            Radio::TxPower power;
            power.set_dB(-12);
            radio_->set_power(power);

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
        neighbor_discovery.init(*radio_, *clock_, *timer_, *debug_, 2000, 16000, echo_threshold, 210);
        debug().debug("nb_init;echo_threshold");
#else    

#endif
        neighbor_discovery.register_debug_callback(nb_t::NEW_NB | nb_t::NEW_NB_BIDI | nb_t::DROPPED_NB | nb_t::LOST_NB_BIDI);
        neighbor_discovery.enable();
    }

    //    void handle_sensor() {
    //        //        debug().debug("pir event from node %x", radio_->id());
    //        if (pir_sensor_) {
    //#ifdef INTEGER_STORAGE
    //            int pir = semantics_t::PIR, value = 1;
    //            semantics_.set_semantic_value(predicate_t((block_data_t*) & pir, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
    //            debug().debug("Sread:%d:%d", pir, value);
    //#endif
    //        }
    //    }

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
            clustering_algo_.init(*radio_, *timer_, *debug_, *rand_, neighbor_discovery, semantics_);
            seQueryRouting_.init(*radio_, *timer_, *debug_, semantics_, neighbor_discovery);

            disabled_ = true;


            if ((radio_->id() >= 0x2000) && (radio_->id() < 0x2100)) {//UZL 5139R1
                int room = (radio_->id() - 0x2000) / 0x4;
                int roomID = 4;
                debug().debug("iS %x - R %d ", radio_->id(), room);
                set_semantic((block_data_t*) & roomID, (block_data_t*) & room);
            } else if ((radio_->id() >= 0x2100) && (radio_->id() < 0x2200)) {//UZL 5148
                int room = (radio_->id() - 0x2100) / 0x4;
                int roomID = 4;
                debug().debug("iS %x - R %d ", radio_->id(), room);
                set_semantic((block_data_t*) & roomID, (block_data_t*) & room);
            }

            enable();

#ifndef ISENSE
            enable();
#endif 


            clustering_algo_.register_debug_callback();

        }

#ifdef ISENSE //update semantics part
#ifdef INTEGER_STORAGE
#ifdef USE_SENSORS

        //        if (pir_sensor_) {
        //            int pir = 215, value = rand()(100);
        //            semantics_.set_semantic_value(predicate_t((block_data_t*) & pir, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
        //            debug().debug("Sread:%d:%d", pir, value);
        //        }
        if (light_sensor_) {

            int light = semantics_t::LIGHT, value = em_->light_sensor()->luminance();
            if ((value < 4000) && (value > 0)) {
                semantics_.set_semantic_value(predicate_t((block_data_t*) & light, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
                //                debug().debug("Sread:%d:%d", light, value);
            }
        }
        if (temp_sensor_) {

            int temp = semantics_t::TEMP, value = em_->temp_sensor()->temperature();
            if ((value < 4000) && (value > 0)) {
                semantics_.set_semantic_value(predicate_t((block_data_t*) & temp, sizeof (int)), value_t((block_data_t*) & value, sizeof (int)));
                //                debug().debug("Sread:%d:%d", temp, value);
            }
        }
#endif
#endif
#endif

        timer_->set_timer<SemanticGroupsApp, &SemanticGroupsApp::start > (60000, this, (void *) 1);

    }

    void handle_uart_msg(uart_size_t len, Os::Uart::block_data_t *data) {
        if (data[0] == 0x2) {
            //if an enable message
            if (!is_otap()) {
                //initialize here as it is not possible in switch
                QueryMsg_t test;
                semantics_t::group_entry_t a;
                if (data[1] == 0x1) {
                    enable();
                } else if (data[1] == 0x2) {
                    disable();
                } else if (data[1] == 0x6) {
                    set_semantic((block_data_t*) data + 2, (block_data_t*) data + 6);
                } else if (data[1] == 0x7) {

                    test.set_msg_id(QueryMsg_t::SEROUTING);
                    test.set_alg_id(QueryMsg_t::QUERY);
                    test.set_sender(radio_->id());
                    test.set_destination(0xffff);

                    int ge[2] = {data[2], data[3]};
                    semantics_t::group_entry_t a = semantics_t::group_entry_t((block_data_t*) ge);

                    test.add_statement(a.data(), a.size());

                    debug().debug("SQ;%x;%s", radio_->id(), a.c_str());
                    seQueryRouting_.send(0xffff, test.length(), (uint8_t*) & test);

                }
            }
        }
    }

private:

    void enable() {
        if (disabled_) {
            debug().debug("ON");
            clustering_algo_.enable();
            seQueryRouting_.enable_radio();
            disabled_ = false;
        }
    }

    void disable() {
        if (!disabled_) {
            debug().debug("OFF");
            clustering_algo_.disable();
            disabled_ = true;
        }
    }

    void set_semantic(block_data_t* id, block_data_t * value) {
        predicate_t pred = predicate_t(id);
        value_t val = value_t(value);
        debug().debug("SS;%s;%s", pred.c_str(), val.c_str());
        semantics_.set_semantic_value(pred, val);
    }

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
    seQueryRouting_t seQueryRouting_;
    bool disabled_;

    semantics_t semantics_;



    bool light_sensor_, temp_sensor_;

#ifdef ISENSE
#ifdef USE_SENSORS
    isense::EnvironmentModule* em_;
    //    isense::PirSensor* pir_;
#endif
#endif

    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;

    Os::Debug debug() {
        return *debug_;
    }
    Os::Clock::self_pointer_t clock_;
    Os::Uart::self_pointer_t uart_;
    Os::Rand::self_pointer_t rand_;
};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, SemanticGroupsApp> example_app;
// --------------------------------------------------------------------------

void application_main(Os::AppMainParameter& value) {
    example_app.init(value);
}

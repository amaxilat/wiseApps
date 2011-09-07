/**
 * Clustering Application Using Echo
 */

#include "external_interface/external_interface.h"
#include <isense/modules/environment_module/environment_module.h>
#include <isense/modules/security_module/pir_sensor.h>


#ifdef SHAWN

#endif
#ifdef ISENSE
#include "external_interface/isense/isense_light_sensor.h"
#endif

#include "algorithms/neighbor_discovery/echo.h"

#include "algorithms/cluster/clustering_types.h"
#include "internal_interface/routing_table/routing_table_static_array.h"
#include "algorithms/routing/flooding/flooding_algorithm.h"



#include "controll_message.h"
#include "report_message.h"



#ifdef SHAWN
#include "external_interface/shawn/shawn_testbedservice_uart.h"
#endif


#define VISUALIZER
#define ENABLE_UART_CL
#define FAILURES_PERCENTAGE 30


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



typedef wiselib::StaticArrayRoutingTable<Os, Os::Radio, 64 > FloodingStaticMap;
typedef wiselib::FloodingAlgorithm<Os, FloodingStaticMap, Os::TxRadio, Os::Debug> routing_t;


typedef wiselib::Echo<Os, Radio, Os::Timer, Os::Debug> nb_t;

typedef Os::Radio::node_id_t node_id_t;
typedef Os::Radio::block_data_t block_data_t;

typedef wiselib::Semantics<Os> semantics_t;

// Replace the first Algorithm name with one from the list in comment
#define SPIT
#ifdef SPIT
#include "algorithms/cluster/spit/spit_core.h"
#endif


#ifdef SPIT

typedef wiselib::SemanticClusterHeadDecision<Os, routing_t, semantics_t> CHD_t;
typedef wiselib::SemanticJoinDecision<Os, routing_t, semantics_t> JD_t;
typedef wiselib::FrontsIterator<Os, routing_t, semantics_t> IT_t;
typedef wiselib::SpitCore<Os, routing_t, CHD_t, JD_t, IT_t, nb_t, semantics_t> clustering_algo_t;
#endif

typedef Os::Uart::size_t uart_size_t;



//#define REMOTE_DEBUG
#ifdef REMOTE_DEBUG
#include "util/pstl/map_static_vector.h"
#include "algorithms/routing/flooding/flooding_algorithm.h"
#include "algorithms/routing/static/static_routing_algorithm.h"
#include "util/wisebed_node_api/remote_debug.h"

typedef wiselib::MapStaticVector<Os, Radio::node_id_t, uint16_t, 64 > node_map_t;
typedef wiselib::FloodingAlgorithm<Os, node_map_t> flooding_t;
typedef wiselib::MapStaticVector<Os, Radio::node_id_t, Radio::node_id_t, 64 > routing_table_t;
typedef wiselib::StaticRoutingAlgorithm<Os, routing_table_t, Radio> routing_t;
typedef wiselib::RemoteDebugModel<Os, Radio, routing_t, flooding_t, Os::Timer> remote_debug_t;
#endif

typedef wiselib::ControllMsg<Os, Radio> ControllMsg_t;
typedef wiselib::ReportMsg<Os, Radio> ReportMsg_t;

class ClusteringFronts :
public isense::Uint32DataHandler,
public isense::Int8DataHandler,
public isense::SensorHandler {
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
        routing_.init(*radio_, *debug_);
        routing_.enable_radio();


        rand_->srand(radio_->id());

        //        radio_->reg_recv_callback<ClusteringFronts, &ClusteringFronts::receive_commands > (this);

        //clustering_algo_.reg_state_changed_callback<ClusteringFronts, &ClusteringFronts::clustering_events > (this);
        //neighbor_discovery.reg_event_callback<ClusteringFronts, &ClusteringFronts::nd_callback > (7, nb_t::NEW_NB | nb_t::NEW_NB_BIDI, this);

        //radio_->reg_recv_callback<this, &this::receive_commands > (this);


#ifdef REMOTE_DEBUG
        flooding_.init();
        routing_.init();

        //debug_->debug("INIT Static Routing for iSense");
        //routing_.add_hop(0x153d, 0x9979);
        //routing_.add_hop(0xa41c, 0x9979);
        remote_debug_.init(*radio_, routing_, flooding_, *timer_, *debug_);
        remote_debug_.set_sink();
        remote_debug_.debug("hello world");
#endif


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
        debug_->debug("pir");
        //        pir_ = new isense::PirSensor(value);
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
        //        debug_->debug("pir event from node %x", radio_->id());
        semantics_.set_semantic_value(semantics_t::PIR, 1);
    }

    bool is_otap() {
        return false;
    }

    // --------------------------------------------------------------------

    void start(void * a) {
        //
        //        if (clustering_algo_.is_child(0x786a)) {
        //            debug_->debug("IsChild 0x786a");
        //        }

        if (a == 0) {
            disabled_ = false;
            neighbor_discovery.init(*radio_, *clock_, *timer_, *debug_, 1000, 10000, 200, 230);
            // set the HeadDecision Module
            clustering_algo_.set_cluster_head_decision(CHD_);
            // set the JoinDecision Module
            clustering_algo_.set_join_decision(JD_);
            // set the Iterator Module
            clustering_algo_.set_iterator(IT_);
            clustering_algo_.init(routing_, *timer_, *debug_, *rand_, neighbor_discovery, semantics_);




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

            set_semantic(4, 3);
            set_semantic(5, 3);


            set_semantic(211, 0);
            //            set_semantic(4, 4);
            //            set_demands(4, 0xff);
            enable();



        } else {



            //            debug_->debug("NBsize %d", neighbor_discovery.stable_nb_size());
            //            debug_->debug("Node %x Joined %d", radio_->id(), clustering_algo_.clusters_joined());
            //clustering_algo_.present_neighbors();


            //            if (!is_gateway()) {
            //                if (clustering_algo_.cluster_id() != 0xffff) {
            //                    ReportMsg_t mess;
            //                    mess.set_report_type(ReportMsg_t::CLUSTER);
            //                    mess.set_cluster_id(clustering_algo_.cluster_id());
            //                    radio_->send(0xffff, mess.size(), (uint8_t*) & mess);
            //                }
            //            }


        }




        //        debug_->debug("Gateways Nodes %d", clustering_algo_.node_count(0));
        //        debug_->debug("Children Nodes %d", clustering_algo_.node_count(1));
        //        semantics_.set_semantic_value(semantics_t::PIR, 0);
        //        semantics_.set_semantic_value(semantics_t::LIGHT, em_->light_sensor()->luminance());
        //        semantics_.set_semantic_value(semantics_t::TEMP, em_->temp_sensor()->temperature());

        timer_->set_timer<ClusteringFronts,
                &ClusteringFronts::start > (4000, this, (void *) 1);

    }

#ifdef ENABLE_UART_CL

    void handle_uart_msg(uart_size_t len, Os::Uart::block_data_t *data) {
        //debug_->debug("Got a uart message %x", radio_->id());

        if (data[0] == 0x2) {
            ControllMsg_t command;
            //if an enable message
            if (!is_otap()) {
                switch (data[1]) {
                    case 0x1:
                        enable();
                        command.set_controll_type(ControllMsg_t::ON);
                        break;
                    case 0x2:
                        disable();
                        command.set_controll_type(ControllMsg_t::OFF);
                        break;
                    case 0x3:
                        fail();
                        command.set_controll_type(ControllMsg_t::FAIL);
                        break;
                    case 0x4:
                        recover();
                        command.set_controll_type(ControllMsg_t::RECOVER);
                        break;
                    case 0x5:
                        change_k(data[2]);
                        command.set_controll_type(ControllMsg_t::CHANGE_K);
                        command.set_payload(data[2]);
                        break;
                    case 0x6:
                        set_semantic(data[2], data[3]);
                        //command.set_controll_type(ControllMsg_t::CHANGE_K);
                        //command.set_payload(data[2]);
                        break;
                    case 0x7:
                        //run a query
                        for (uint8_t pos = 2; pos + 1 < len; pos += 2) {
                            set_demands(data[pos], data[pos + 1]);
                        }
                        query();
                        break;

                }
                //                radio_->send(0xffff, command.size(), (uint8_t *) & command);
            }
        }
    }
#endif

    //    void receive_commands(Os::Radio::node_id_t src, Os::Radio::size_t len, Os::Radio::block_data_t * mess) {
    //        if (!is_otap()) {
    //            ReportMsg_t *m = (ReportMsg_t *) mess;
    //            if (m->msg_id() == ReportMsg_t::REPORT_MSG) {
    //                switch (m->report_type()) {
    //                    case ReportMsg_t::CLUSTER:
    //                        debug_->debug("Got cluster %x from %x ", m->cluster_id(), src);
    //                        if (src == m->cluster_id()) {
    //                            debug_->debug("CLP;%x;2;%x", src, m->cluster_id());
    //                        } else {
    //                            debug_->debug("CLP;%x;1;%x", src, m->cluster_id());
    //                        }
    //                        return;
    //                    case ReportMsg_t::CLUSTER_MESSAGE:
    //                        debug_->debug("CLS;%x;45;%x", src, 0xffff);
    //                        return;
    //                    case ReportMsg_t::NEW_NB:
    //                        debug_->debug("NB;%x;%x", m->node_id(), src);
    //                        return;
    //                    case ReportMsg_t::NEW_NB_BIDI:
    //                        debug_->debug("NBB;%x;%x", m->node_id(), src);
    //                        return;
    //                    case ReportMsg_t::DROPPED_NB:
    //                        debug_->debug("NBD;%x;%x", m->node_id(), src);
    //                        return;
    //                    case ReportMsg_t::LOST_NB_BIDI:
    //                        debug_->debug("NBL;%x;%x", m->node_id(), src);
    //                        return;
    //                    default:
    //                        return;
    //                }
    //            }
    //            return;
    //        } else {
    //            ControllMsg_t *m = (ControllMsg_t *) mess;
    //            if (m->msg_id() == ControllMsg_t::CONTROLL_MSG) {
    //                switch (m->controll_type()) {
    //                    case ControllMsg_t::ON:
    //                        enable();
    //                        return;
    //                    case ControllMsg_t::OFF:
    //                        disable();
    //                        return;
    //                    case ControllMsg_t::FAIL:
    //                        fail();
    //                        return;
    //                    case ControllMsg_t::RECOVER:
    //                        recover();
    //                        return;
    //                    case ControllMsg_t::CHANGE_K:
    //                        change_k(m->payload());
    //                        return;
    //
    //                    default:
    //                        return;
    //                }
    //            }
    //        }
    //    }

    //    void clustering_events(int event) {
    //        if (is_otap()) {
    //            ReportMsg_t mess;
    //            mess.set_msg_id(ReportMsg_t::REPORT_MSG);
    //            switch (event) {
    //                case wiselib::CLUSTER_FORMED:
    //                case wiselib::NODE_JOINED:
    //                case wiselib::ELECTED_CLUSTER_HEAD:
    //                    mess.set_report_type(ReportMsg_t::CLUSTER);
    //                    mess.set_cluster_id(clustering_algo_.cluster_id());
    //                    radio_->send(0xffff, mess.size(), (uint8_t*) & mess);
    //                    return;
    //                case wiselib::MESSAGE_SENT:
    //                    mess.set_report_type(ReportMsg_t::CLUSTER_MESSAGE);
    //                    radio_->send(0xffff, mess.size(), (uint8_t*) & mess);
    //                    return;
    //                default:
    //                    return;
    //            }
    //        } else {
    //            switch (event) {
    //                case wiselib::CLUSTER_FORMED:
    //                case wiselib::NODE_JOINED:
    //                case wiselib::ELECTED_CLUSTER_HEAD:
    //                    if (clustering_algo_.cluster_id() == radio_->id()) {
    //                        debug_->debug("CLP;%x;2;%x", radio_->id(), clustering_algo_.cluster_id());
    //                    } else {
    //                        debug_->debug("CLP;%x;1;%x", radio_->id(), clustering_algo_.cluster_id());
    //                    }
    //                    return;
    //                case wiselib::MESSAGE_SENT:
    //                    debug_->debug("CLS;%x;45;%x", radio_->id(), 0xffff);
    //                    return;
    //                default:
    //                    return;
    //            }
    //        }
    //    }

    //    void nd_callback(uint8_t event, node_id_t from, uint8_t len, uint8_t * data) {
    //        if (disabled_) return;
    //        if (is_otap()) {
    //            ReportMsg_t mess;
    //            mess.set_msg_id(ReportMsg_t::REPORT_MSG);
    //            switch (event) {
    //                case nb_t::DROPPED_NB:
    //                    mess.set_report_type(ReportMsg_t::DROPPED_NB);
    //                    break;
    //                case nb_t::LOST_NB_BIDI:
    //                    mess.set_report_type(ReportMsg_t::LOST_NB_BIDI);
    //                    break;
    //                case nb_t::NEW_NB_BIDI:
    //                    mess.set_report_type(ReportMsg_t::NEW_NB_BIDI);
    //                    break;
    //                case nb_t::NEW_NB:
    //                    mess.set_report_type(ReportMsg_t::NEW_NB);
    //                    break;
    //                default:
    //                    return;
    //            }
    //            mess.set_node_id(from);
    //            radio_->send(0xffff, mess.size(), (uint8_t *) & mess);
    //        } else {
    //            switch (event) {
    //                case nb_t::DROPPED_NB:
    //                    debug_->debug("NBD;%x;%x", from, radio_->id());
    //                    break;
    //                case nb_t::LOST_NB_BIDI:
    //                    debug_->debug("NBL;%x;%x", from, radio_->id());
    //                    break;
    //                case nb_t::NEW_NB_BIDI:
    //                    debug_->debug("NBB;%x;%x", from, radio_->id());
    //                    break;
    //                case nb_t::NEW_NB:
    //                    debug_->debug("NB;%x;%x", from, radio_->id());
    //                    break;
    //                default:
    //                    return;
    //            }
    //        }
    //    }


private:

    void enable() {
        if (disabled_) {
            debug_->debug("ON");
            //            neighbor_discovery.enable();
            clustering_algo_.enable(5);
            disabled_ = false;
        }
    }

    void disable() {
        if (!disabled_) {
            debug_->debug("OFF");
            neighbor_discovery.disable();
            clustering_algo_.disable();
            disabled_ = true;
        }
    }

    void fail() {
        if (!disabled_) {
            if ((*rand_)() % 100 < FAILURES_PERCENTAGE) {
                debug_->debug("Failing;%x", radio_->id());
                neighbor_discovery.disable();
                clustering_algo_.disable();
                disabled_ = true;
            }
        }
    }

    void recover() {
        if (disabled_) {
            debug_->debug("Recovering;%x", radio_->id());
            //            neighbor_discovery.enable();
            clustering_algo_.enable(20);
            disabled_ = false;
        }
    }

    void change_k(uint8_t k) {
        debug_->debug("ChangeK;%x;%d", radio_->id(), k);
        //        clustering_algo_.set_maxhops(k);
    }

    void set_semantic(int id, int value) {
        debug_->debug("SS;%d;%d", id, value);
        semantics_.set_semantic(id, value);
    }

    void set_demands(int id, int value) {
        debug_->debug("SD;%d;%d", id, value);
        clustering_algo_.set_demands(id, value);

        //        semantics_.set_semantic_value(semantics_t::PIR, 0);
        //        semantics_.set_semantic_value(semantics_t::LIGHT, em_->light_sensor()->luminance());
        //        semantics_.set_semantic_value(semantics_t::TEMP, em_->temp_sensor()->temperature());
    }

    void query() {
        clustering_algo_.answer(0);
    }

    nb_t neighbor_discovery;
    routing_t routing_;
    
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

#ifdef REMOTE_DEBUG
    remote_debug_t remote_debug_;
    flooding_t flooding_;
    routing_t routing_;
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

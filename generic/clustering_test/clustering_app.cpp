/**
 * Clustering Application
 */

#ifdef ISENSE
extern "C" {

    void assert(int) {
    }
}
#endif 

#include "configuration.h"

#include "external_interface/external_interface.h"
#include "external_interface/external_interface_testing.h"

#ifdef ECHO
#include "algorithms/neighbor_discovery/echo.h"
#define ECHO_ALG Echo
#endif
#ifdef ADAPTIVE
#include "algorithms/neighbor_discovery/adaptive/adaptive_nd.h"
#define ECHO_ALG AdaptiveND
#endif

#include "algorithms/cluster/clustering_types.h"


#ifdef FRONTS
#include "algorithms/cluster/fronts/fronts_core.h"
#endif
#ifdef MOCA
#include "algorithms/cluster/moca/moca.h"
#endif
#ifdef LCA
#include "algorithms/cluster/lca/lca.h"
#endif

#include "controll_message.h"
#include "report_message.h"
#include "external_interface/isense/isense_facet_provider.h"

typedef wiselib::OSMODEL Os;


#ifdef VIRTUAL_RADIO
#include "util/wisebed_node_api/virtual_extended_txradio.h"
#include "util/base_classes/routing_base.h"
typedef Os::TxRadio hRadio;
typedef wiselib::VirtualExtendedTxRadioModel<Os, Os::TxRadio, Os::Uart> Radio;
#else 
typedef Os::TxRadio Radio;
#endif

#ifdef ECHO
typedef wiselib::ECHO_ALG<Os, Radio, Os::Timer, Os::Debug> nb_t;
#endif
#ifdef ADAPTIVE
typedef wiselib::ECHO_ALG<Os, Radio, Os::Timer, Os::Debug, Os::Rand, Os::DutyCycling> nb_t;
#endif


typedef Os::Radio::node_id_t node_id_t;
typedef Os::Radio::block_data_t block_data_t;

#ifdef FRONTS
typedef wiselib::AtributeClusterHeadDecision<Os, Radio> chd_t;
typedef wiselib::FronstDecision<Os, Radio> jd_t;
typedef wiselib::FrontsIterator<Os, Radio> it_t;
typedef wiselib::FrontsCore<Os, Radio, chd_t, jd_t, it_t, nb_t> clustering_algo_t;
#endif
#ifdef MOCA
typedef wiselib::ProbabilisticClusterHeadDecision<Os, Radio> chd_t;
typedef wiselib::MocaJoinDecision<Os, Radio> jd_t;
typedef wiselib::OverlappingIterator<Os, Radio> it_t;
typedef wiselib::MocaCore<Os, Radio, chd_t, jd_t, it_t> clustering_algo_t;
#endif
#ifdef LCA
typedef wiselib::ProbabilisticClusterHeadDecision<Os, Radio> chd_t;
typedef wiselib::BfsJoinDecision<Os, Radio> jd_t;
typedef wiselib::FrontsIterator<Os, Radio> it_t;
typedef wiselib::LcaCore<Os, Radio, chd_t, jd_t, it_t, nb_t> clustering_algo_t;
#endif

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

class ClusteringFronts : public isense::SleepHandler{
public:
#ifdef ENABLE_UART_CL
    //typedef Os::Uart::size_t uart_size_t;
#endif
    
            /**
         * Called before going to sleep.
         * @return true if the node can go sleep.
         */
        bool stand_by() {
            debug_->debug("Going to sleep");
            return true;
        }

        /**
         * Called when waking up.
         * @param state previous state.
         */
        void wake_up(bool state) {
            debug_->debug("waking up");
        }

    void init(Os::AppMainParameter& value) {

        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);
        clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet(value);
#ifdef ENABLE_UART_CL
        uart_ = &wiselib::FacetProvider<Os, Os::Uart>::get_facet(value);
#endif
        rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet(value);
        duty_ = &wiselib::FacetProvider<Os, Os::DutyCycling>::get_facet(value);               

#ifdef VIRTUAL_RADIO
        radio_ = &virtual_radio_;
        hardware_radio_ = &wiselib::FacetProvider<Os, Os::TxRadio>::get_facet(value);
        virtual_radio_.init(*hardware_radio_, *uart_, *debug_);

#else 
        radio_ = &wiselib::FacetProvider<Os, Os::TxRadio>::get_facet(value);
#endif

        rand_->srand(radio_->id());

        radio_->reg_recv_callback<ClusteringFronts, &ClusteringFronts::receive_commands > (this);

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

#ifndef SHAWN
#ifdef CHANGE_POWER
        Radio::TxPower power;
        power.set_dB(DB);
        radio_->set_power(power);
#endif
        radio_->set_channel(CHANNEL);

#ifdef ENABLE_UART_CL
        uart_->reg_read_callback<ClusteringFronts, &ClusteringFronts::handle_uart_msg > (this);
        uart_->enable_serial_comm();
#endif

#endif
        debug_->debug("*Boot%x*", radio_->id());
        timer_->set_timer<ClusteringFronts, &ClusteringFronts::start > (1000, this, 0);
    }

    // --------------------------------------------------------------------

    bool is_otap() {
        return false;
    }

    // --------------------------------------------------------------------

    void start(void * a) {
        int b = (int) a;

        if (b == 0) {
            disabled_ = false;
#ifdef ECHO
            neighbor_discovery.init(*radio_, *clock_, *timer_, *debug_, 1000, 16000, 200, 240);
#endif
#ifdef ADAPTIVE
            neighbor_discovery.init(*radio_, *clock_, *timer_, *debug_, *rand_, *duty_, DUTY_PERIOD, DUTY_RATE);
#endif
#ifdef ENABLE_CLUSTERING
            // set the HeadDecision Module
            clustering_algo_.set_cluster_head_decision(chd_);
            // set the JoinDecision Module
            clustering_algo_.set_join_decision(jd_);
            // set the Iterator Module
            clustering_algo_.set_iterator(it_);
            clustering_algo_.init(*radio_, *timer_, *debug_, *rand_, neighbor_discovery);

            clustering_algo_.set_maxhops(2);
#ifdef MOCA
            clustering_algo_.set_probability(40);
#endif
#ifdef LCA
            clustering_algo_.set_probability(10);
#endif
#endif

            disabled_ = true;

            //            if (!is_otap()) {
            neighbor_discovery.register_debug_callback(0);
#ifdef ENABLE_CLUSTERING
            clustering_algo_.register_debug_callback();
#endif
            //            }

            enable();
            timer_->set_timer<ClusteringFronts, &ClusteringFronts::start > (5 * 60000, this, (void *) 1);
        } else if (b == 1) {
            fail();
            timer_->set_timer<ClusteringFronts, &ClusteringFronts::start > (5 * 60000, this, (void *) 2);
        } else {
            debug_->debug("Deathnote2");
            recover();
        }

    }

#ifdef ENABLE_UART_CL

    void handle_uart_msg(uart_size_t len, Os::Uart::block_data_t *data) {
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
                }
                radio_->send(Radio::BROADCAST_ADDRESS, command.size(), (uint8_t *) & command);
            }
        }
    }
#endif

    void receive_commands(node_id_t src, Radio::size_t len, block_data_t * mess) {
        if (!is_otap()) {
            ReportMsg_t *m = (ReportMsg_t *) mess;
            if (m->msg_id() == ReportMsg_t::REPORT_MSG) {
                switch (m->report_type()) {
                    case ReportMsg_t::CLUSTER:
                        debug_->debug("Got cluster %x from %x ", m->cluster_id(), src);
                        if (src == m->cluster_id()) {
                            debug_->debug("CLP;%x;2;%x", src, m->cluster_id());
                        } else {
                            debug_->debug("CLP;%x;1;%x", src, m->cluster_id());
                        }
                        return;
                    case ReportMsg_t::CLUSTER_MESSAGE:
                        debug_->debug("CLS;%x;45;%x", src, 0xffff);
                        return;
                    case ReportMsg_t::NEW_NB:
                        debug_->debug("NB;%x;%x", m->node_id(), src);
                        return;
                    case ReportMsg_t::NEW_NB_BIDI:
                        debug_->debug("NBB;%x;%x", m->node_id(), src);
                        return;
                    case ReportMsg_t::DROPPED_NB:
                        debug_->debug("NBD;%x;%x", m->node_id(), src);
                        return;
                    case ReportMsg_t::LOST_NB_BIDI:
                        debug_->debug("NBL;%x;%x", m->node_id(), src);
                        return;
                    default:
                        return;
                }
            }
            return;
        } else {
            ControllMsg_t *m = (ControllMsg_t *) mess;
            if (m->msg_id() == ControllMsg_t::CONTROLL_MSG) {
                switch (m->controll_type()) {
                    case ControllMsg_t::ON:
                        enable();
                        return;
                    case ControllMsg_t::OFF:
                        disable();
                        return;
                    case ControllMsg_t::FAIL:
                        fail();
                        return;
                    case ControllMsg_t::RECOVER:
                        recover();
                        return;
                    case ControllMsg_t::CHANGE_K:
                        change_k(m->payload());
                        return;

                    default:
                        return;
                }
            }
        }
    }

    void clustering_events(int event) {
#ifdef ENABLE_CLUSTERING
        if (is_otap()) {
            ReportMsg_t mess;
            mess.set_msg_id(ReportMsg_t::REPORT_MSG);
            switch (event) {
                case wiselib::CLUSTER_FORMED:
                case wiselib::NODE_JOINED:
                case wiselib::ELECTED_CLUSTER_HEAD:
                    mess.set_report_type(ReportMsg_t::CLUSTER);
                    mess.set_cluster_id(clustering_algo_.cluster_id());
                    radio_->send(0xffff, mess.size(), (uint8_t*) & mess);
                    return;
                case wiselib::MESSAGE_SENT:
                    mess.set_report_type(ReportMsg_t::CLUSTER_MESSAGE);
                    radio_->send(0xffff, mess.size(), (uint8_t*) & mess);
                    return;
                default:
                    return;
            }
        } else {
            switch (event) {
                case wiselib::CLUSTER_FORMED:
                case wiselib::NODE_JOINED:
                case wiselib::ELECTED_CLUSTER_HEAD:
                    if (clustering_algo_.cluster_id() == radio_->id()) {
                        debug_->debug("CLP;%x;2;%x", radio_->id(), clustering_algo_.cluster_id());
                    } else {
                        debug_->debug("CLP;%x;1;%x", radio_->id(), clustering_algo_.cluster_id());
                    }
                    return;
                case wiselib::MESSAGE_SENT:
                    debug_->debug("CLS;%x;45;%x", radio_->id(), 0xffff);
                    return;
                default:
                    return;
            }
        }
#endif
    }

    void nd_callback(uint8_t event, node_id_t from, uint8_t len, uint8_t * data) {
        if (disabled_) return;
        if (is_otap()) {
            ReportMsg_t mess;
            mess.set_msg_id(ReportMsg_t::REPORT_MSG);
            switch (event) {
                case nb_t::DROPPED_NB:
                    mess.set_report_type(ReportMsg_t::DROPPED_NB);
                    break;
                case nb_t::LOST_NB_BIDI:
                    mess.set_report_type(ReportMsg_t::LOST_NB_BIDI);
                    break;
                case nb_t::NEW_NB_BIDI:
                    mess.set_report_type(ReportMsg_t::NEW_NB_BIDI);
                    break;
                case nb_t::NEW_NB:
                    mess.set_report_type(ReportMsg_t::NEW_NB);
                    break;
                default:
                    return;
            }
            mess.set_node_id(from);
            radio_->send(0xffff, mess.size(), (uint8_t *) & mess);
        } else {
            switch (event) {
                case nb_t::DROPPED_NB:
                    debug_->debug("NBD;%x;%x", from, radio_->id());
                    break;
                case nb_t::LOST_NB_BIDI:
                    debug_->debug("NBL;%x;%x", from, radio_->id());
                    break;
                case nb_t::NEW_NB_BIDI:
                    debug_->debug("NBB;%x;%x", from, radio_->id());
                    break;
                case nb_t::NEW_NB:
                    debug_->debug("NB;%x;%x", from, radio_->id());
                    break;
                default:
                    return;
            }
        }
    }


private:

    void enable() {
        if (disabled_) {
            debug_->debug("ON");
            neighbor_discovery.enable();
#ifdef ENABLE_CLUSTERING
            clustering_algo_.enable(40);
#endif
            disabled_ = false;
        }
    }

    void disable() {
        if (!disabled_) {
            debug_->debug("OFF");
            neighbor_discovery.disable();
#ifdef ENABLE_CLUSTERING
            clustering_algo_.disable();
#endif
            disabled_ = true;
        }
    }

    bool fail() {
        if (!disabled_) {
            int randnum = (*rand_)() % 10;
            debug_->debug("Deathnote:%d", randnum);
            if (randnum < FAILURES_PERCENTAGE) {
                debug_->debug("Failing;%x", radio_->id());
                neighbor_discovery.disable();
#ifdef ENABLE_CLUSTERING
                clustering_algo_.disable();
#endif
                disabled_ = true;
                return true;
            }
        }
        return false;
    }

    bool recover() {
        if (disabled_) {
            //            if ((*rand_)() % 10 > FAILURES_PERCENTAGE) {
            debug_->debug("Recovering;%x", radio_->id());
            neighbor_discovery.enable();
#ifdef ENABLE_CLUSTERING
            clustering_algo_.enable(20);
#endif
            disabled_ = false;
            return true;
            //            }
        }
        return false;
    }

    void change_k(uint8_t k) {
        debug_->debug("ChangeK;%x;%d", radio_->id(), k);
#ifdef ENABLE_CLUSTERING
        clustering_algo_.set_maxhops(k);
#endif
    }

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
#ifdef ENABLE_CLUSTERING
    // clustering algorithm modules
    chd_t chd_;
    jd_t jd_;
    it_t it_;
    // clustering algorithm core component
    clustering_algo_t clustering_algo_;
#endif
    bool disabled_;
    bool clustering_enabled_;

    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;
    Os::Clock::self_pointer_t clock_;
#ifdef ENABLE_UART_CL
    Os::Uart::self_pointer_t uart_;
#endif
    Os::Rand::self_pointer_t rand_;

    Os::DutyCycling::self_pointer_t duty_;
};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ClusteringFronts> example_app;
// --------------------------------------------------------------------------

void application_main(Os::AppMainParameter& value) {
    example_app.init(value);
}

#include "external_interface/external_interface.h"

#include "algorithms/cluster/clustering_types.h"

#define ENABLE_UART_CL

typedef wiselib::OSMODEL Os;
typedef Os::Radio::node_id_t node_id_t;
typedef Os::block_data_t block_data_t;


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


typedef Os::Uart::size_t uart_size_t;

typedef semantics_t::predicate_t predicate_t;
typedef semantics_t::group_container_t group_container_t;
typedef semantics_t::value_t value_t;

class SemanticGroupsApp {
public:

    void init(Os::AppMainParameter& value) {

        radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet(value);
        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);

#ifdef ENABLE_UART_CL
        uart_ = &wiselib::FacetProvider<Os, Os::Uart>::get_facet(value);
#endif




        debug_->debug("*B*%x", radio_->id());




#ifdef ENABLE_UART_CL
        uart_->reg_read_callback<SemanticGroupsApp, &SemanticGroupsApp::handle_uart_msg > (this);
        uart_->enable_serial_comm();
#endif

        timer_->set_timer<SemanticGroupsApp, &SemanticGroupsApp::add_semantics > (1000, this, 0);

    }

    // --------------------------------------------------------------------

    void add_semantics(void * a) {
        //
        //        if (clustering_algo_.is_child(0x786a)) {
        //            debug_->debug("IsChild 0x786a");
        //        }

        if (a == 0) {
            debug_->debug("ON");

            debug_->debug("adding semantic");
            for (int i = 1; i < 255; i++) {
#ifdef INTEGER_STORAGE
                uint8_t pred = i;
                size_t predSize = sizeof (uint8_t);
                uint8_t val = i;
                size_t valSize = sizeof (uint8_t);
                predicate_t predicate = predicate_t((block_data_t *) & pred, predSize, semantics_.get_allocator());
                value_t value = value_t((block_data_t *) & val, valSize, semantics_.get_allocator());

#else 
                const unsigned char predicate_str[] = "room";
                predicate_t predicate = predicate_t(predicate_str, semantics_.get_allocator());
                const unsigned char value_str[] = "4";
                value_t value = value_t(value_str, semantics_.get_allocator());
#endif
                semantics_.set_semantic_value(predicate, value);
            }
        }
        debug_->debug("added semantic");


        debug_->debug("reading predicates");
        //            group_container_t preds = semantics_.get_groups();
        debug_->debug("read predicates");



        timer_->set_timer<SemanticGroupsApp,
                &SemanticGroupsApp::list_semantics > (1000, this, (void *) 0);
        //        }
    }

    // --------------------------------------------------------------------

    void list_semantics(void * a) {
        debug_->debug("displaying group entries");
        group_container_t groups = semantics_.get_groups();
        for (group_container_t::iterator it = groups.begin(); it != groups.end(); ++it) {
            debug_->debug("group_entry: %s", it->c_str());
        }
    }

#ifdef ENABLE_UART_CL

    void handle_uart_msg(uart_size_t len, Os::Uart::block_data_t *data) {
        if (data[0] == 0x2) {

        }
    }

#endif

private:

    semantics_t semantics_;

    Os::Radio::self_pointer_t radio_;
    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;

#ifdef ENABLE_UART_CL
    Os::Uart::self_pointer_t uart_;
#endif

};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, SemanticGroupsApp> example_app;
// --------------------------------------------------------------------------

void application_main(Os::AppMainParameter& value) {
    example_app.init(value);
}

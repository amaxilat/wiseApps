#include "external_interface/external_interface.h"

#include "algorithms/cluster/clustering_types.h"

#define ENABLE_UART_CL

typedef wiselib::OSMODEL Os;
typedef Os::Radio::node_id_t node_id_t;
typedef Os::block_data_t block_data_t;


typedef Os::Uart::size_t uart_size_t;

class SemanticGroupsApp {
public:

    void init(Os::AppMainParameter& value) {

        radio_ = &wiselib::FacetProvider<Os, Os::Radio>::get_facet(value);
        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);

        debug_->debug("*Boot*%x", radio_->id());

        rand_ = &wiselib::FacetProvider<Os, Os::Rand>::get_facet(value);

#ifdef ENABLE_UART_CL

        uart_ = &wiselib::FacetProvider<Os, Os::Uart>::get_facet(value);

        uart_->reg_read_callback<SemanticGroupsApp, &SemanticGroupsApp::handle_uart_msg > (this);
        uart_->enable_serial_comm();
#endif

        timer_->set_timer<SemanticGroupsApp, &SemanticGroupsApp::testDouble > (1000, this, 0);

    }

    // --------------------------------------------------------------------

    void testDouble(void *) {
        double a = rand()(100);
        double b = rand()(100);
        double c = a / b;

        debug_->debug("calculating division of %f / %f = %f", a, b, c);

        timer_->set_timer<SemanticGroupsApp,
                &SemanticGroupsApp::customTest > (1000, this, (void *) 0);

    }

    void customTest(void *) {
        float c_ = rand()(100), k_ = rand()(100);
        float temp_k = 0.7 * c_ + 0.3 * k_;

        int int_k = (int) temp_k;

        if (int_k + 0.5 <= temp_k) {
            debug_->debug("ID:%x cond_if(int_k +0.5 <=temp_k) -> true", radio_->id());
            temp_k = temp_k + 1;
        }

        k_ = (int) temp_k;
        debug_->debug("ID:%x k=%d", radio_->id(), k_);

        timer_->set_timer<SemanticGroupsApp,
                &SemanticGroupsApp::loopingTest > (1000, this, (void *) 0);
    }

    void loopingTest(void *) {
        timer_->set_timer<SemanticGroupsApp,
                &SemanticGroupsApp::loopingTest > (1000, this, (void *) 0);
    }


#ifdef ENABLE_UART_CL

    void handle_uart_msg(uart_size_t len, Os::Uart::block_data_t *data) {
        debug_->debug("received uart payload of length %d ", len);
    }

#endif

private:

    Os::Radio::self_pointer_t radio_;
    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;
    Os::Rand::self_pointer_t rand_;

    Os::Rand rand() {
        return *rand_;
    }

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

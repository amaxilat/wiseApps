
#include "external_interface/external_interface_testing.h"
#include "isense/os.h"

typedef wiselib::OSMODEL Os;
typedef Os::TxRadio Radio;
typedef Radio::block_data_t block_data_t;
typedef Os::Radio::node_id_t node_id_t;
typedef Os::Uart::size_t uart_size_t;
typedef Os::Clock Clock;

class ExampleApplication : public isense::SleepHandler {
public:

    void init(Os::AppMainParameter& value) {
        //    os_ = &wiselib::FacetProvider<Os, Os>::get_facet( value );

        radio_ = &wiselib::FacetProvider<Os, Radio>::get_facet(value);
        timer_ = &wiselib::FacetProvider<Os, Os::Timer>::get_facet(value);
        debug_ = &wiselib::FacetProvider<Os, Os::Debug>::get_facet(value);
        duty_ = &wiselib::FacetProvider<Os, Os::DutyCycling>::get_facet(value);
        clock_ = &wiselib::FacetProvider<Os, Os::Clock>::get_facet(value);

        radio_->reg_recv_callback<ExampleApplication, &ExampleApplication::receive > (this);
        radio_->set_channel(15);

        debug_->debug("id : %x", radio_->id());

        if (radio_->id() == 0x153d) {
            timer_->set_timer<ExampleApplication, &ExampleApplication::send > (1000, this, 0);
        }

        if (radio_->id() == 0x153d) {
            wake_up_period_ = 0;
            sleep_period_ = 0;
        } else {
            wake_up_period_ = 5000;
            sleep_period_ = 5000;
        }
        timer_->set_timer<ExampleApplication, &ExampleApplication::set_sleep > (10000, this, 0);
    }

    bool stand_by() {
        return true;
    }

    void wake_up(bool state) {
    }

    void set_sleep(void *) {
        if ((sleep_period_ > 0) && (wake_up_period_ > 0)) {
            timer_->set_timer<ExampleApplication, &ExampleApplication::change_sleep > (wake_up_period_, this, (void *) 0);
        }
        if ((sleep_period_ == 0) && (wake_up_period_ == 0)) {
            timer_->set_timer<ExampleApplication, &ExampleApplication::change_sleep > (10000, this, (void *) 1);
        } else {
            timer_->set_timer<ExampleApplication, &ExampleApplication::change_sleep > (wake_up_period_ + sleep_period_, this, (void *) 1);
        }
    }

    void change_sleep(void * action) {
        if ((action == (void*) 0)) {
            duty_->sleep();
        } else {
            duty_->wake();
            set_sleep(0);
        }
    }

    void send(void * a) {
        uint8_t buf[1];
        buf[0] = 2;
        radio_->send(0xffff, 1, buf);
        timer_->set_timer<ExampleApplication, &ExampleApplication::send > (1000, this, 0);
    }

    void receive(node_id_t src, Radio::size_t len, block_data_t * mess) {
        debug_->debug("node %x received from %x", radio_->id(), src);
    }


private:
    uint16_t wake_up_period_;
    uint16_t sleep_period_;
    isense::Os * os;
    Os os_;
    Radio::self_pointer_t radio_;
    Os::Timer::self_pointer_t timer_;
    Os::Debug::self_pointer_t debug_;
    Os::Clock::self_pointer_t clock_;
    Os::Rand::self_pointer_t rand_;
    Os::DutyCycling::self_pointer_t duty_;
};
// --------------------------------------------------------------------------
wiselib::WiselibApplication<Os, ExampleApplication> example_app;
// --------------------------------------------------------------------------

void application_main(Os::AppMainParameter& value) {
    example_app.init(value);

}

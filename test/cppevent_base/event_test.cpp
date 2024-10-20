#include <doctest/doctest.h>

#include <cppevent_base/event_bus.hpp>
#include <cppevent_base/event_callback.hpp>
#include <cppevent_base/task.hpp>

#define SAMPLE_NUM 100
#define MULTIPLIER 2

cppevent::task<> multiply_num(int& num, cppevent::event_callback& cb) {
    num *= co_await cb.await_status();
}

TEST_CASE("event test") {
    cppevent::event_bus bus;
    cppevent::event_callback cb = bus.get_event_callback();

    SUBCASE("trigger event") {
        int num = SAMPLE_NUM;
        auto t = multiply_num(num, cb);
        CHECK(num == SAMPLE_NUM);
        bus.notify(cb.get_id(), MULTIPLIER);
        CHECK(num == SAMPLE_NUM * MULTIPLIER);
    }

}

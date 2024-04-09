#include <doctest/doctest.h>

#include <cppevent_base/event_bus.hpp>
#include <cppevent_base/event_callback.hpp>

#define SAMPLE_NUM 100

TEST_CASE("event_bus test") {
    cppevent::event_bus bus;

    SUBCASE("status update") {
        cppevent::event_callback cb { bus };
        bus.notify(cb.get_id(), SAMPLE_NUM);
        CHECK_EQ(cb.get_status(), SAMPLE_NUM);
    }

    SUBCASE("no status update") {
        cppevent::event_callback cb { bus };
        bus.notify(cb.get_id() * 10, SAMPLE_NUM);
        CHECK_FALSE(cb.has_status());
    }
}

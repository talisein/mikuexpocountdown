#include <glib.h>
#include <chrono>
#include "events.hpp"

static void test_properties_registered() {
    GObjectClass *klass = (GObjectClass *)g_type_class_ref(
        (GType)Miku::Event::_peel_get_type());

    GParamSpec *p;

    p = g_object_class_find_property(klass, "name");
    g_assert_nonnull(p);
    g_assert_true(G_PARAM_SPEC_VALUE_TYPE(p) == G_TYPE_STRING);

    p = g_object_class_find_property(klass, "style-class");
    g_assert_nonnull(p);
    g_assert_true(G_PARAM_SPEC_VALUE_TYPE(p) == G_TYPE_STRING);

    p = g_object_class_find_property(klass, "start-time");
    g_assert_nonnull(p);
    g_assert_true(G_PARAM_SPEC_VALUE_TYPE(p) == G_TYPE_INT64);

    p = g_object_class_find_property(klass, "end-time");
    g_assert_nonnull(p);
    g_assert_true(G_PARAM_SPEC_VALUE_TYPE(p) == G_TYPE_INT64);

    p = g_object_class_find_property(klass, "timezone-name");
    g_assert_nonnull(p);
    g_assert_true(G_PARAM_SPEC_VALUE_TYPE(p) == G_TYPE_STRING);

    g_type_class_unref(klass);
}

static void test_string_properties() {
    auto events = Miku::get_events();
    g_assert_false(std::ranges::empty(events));
    const auto &e = events.front();

    auto name = e->get_property(Miku::Event::prop_name());
    g_assert_cmpstr(name, ==, e->name());

    auto style_class = e->get_property(Miku::Event::prop_style_class());
    g_assert_cmpstr(style_class, ==, e->style_class());

    auto tz_name = e->get_property(Miku::Event::prop_timezone_name());
    g_assert_cmpstr(tz_name, ==, e->start_time().get_time_zone()->name().data());
}

static void test_time_properties() {
    auto events = Miku::get_events();
    g_assert_false(std::ranges::empty(events));
    const auto &e = events.front();

    int64_t start = e->get_property(Miku::Event::prop_start_time());
    int64_t expected_start = static_cast<int64_t>(
        e->start_time().get_sys_time().time_since_epoch().count());
    g_assert_true(start == expected_start);

    int64_t end_t = e->get_property(Miku::Event::prop_end_time());
    int64_t expected_end = static_cast<int64_t>(
        e->end_time().get_sys_time().time_since_epoch().count());
    g_assert_true(end_t == expected_end);

    g_assert_true(end_t > start);
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/event-properties/registration", test_properties_registered);
    g_test_add_func("/event-properties/strings",      test_string_properties);
    g_test_add_func("/event-properties/times",        test_time_properties);
    return g_test_run();
}

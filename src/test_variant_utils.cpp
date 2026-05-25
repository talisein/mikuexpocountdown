#include <glib.h>
#include <peel/GLib/VariantBuilder.h>
#include "variant_utils.hpp"

static void test_string_roundtrip() {
    auto v = peel::GLib::Variant::create<const char*>("hello");

    // peel::String must be stored before passing to g_assert_cmpstr; the macro
    // captures const char* before the temporary peel::String would be destroyed.
    peel::String ps = variant_to_peel_string(v);
    g_assert_cmpstr(ps, ==, "hello");

    std::string ss = variant_to_string(v);
    g_assert_cmpstr(ss.c_str(), ==, "hello");

    auto sv = variant_to_string_view(v);
    g_assert_true(sv == "hello");

    // Also test with raw Variant* (FloatPtr implicit conversion)
    peel::String ps2 = variant_to_peel_string(static_cast<peel::GLib::Variant*>(v));
    g_assert_cmpstr(ps2, ==, "hello");
}

static void test_empty_string() {
    auto v = peel::GLib::Variant::create<const char*>("");
    auto sv = variant_to_string_view(v);
    g_assert_true(sv.empty());
    g_assert_nonnull(sv.data());    // empty string, not null
}

static void test_child_value() {
    // Simulates parse_string_array — get_child_value returns RefPtr<Variant>
    auto builder = peel::GLib::Variant::Builder::create(
        reinterpret_cast<const peel::GLib::Variant::Type*>(G_VARIANT_TYPE("as")));
    builder->add("s", "foo");
    auto as_var = builder->end();
    auto child = static_cast<peel::GLib::Variant*>(as_var)->get_child_value(0);
    // RefPtr<Variant> overload
    peel::String ps = variant_to_peel_string(child);
    g_assert_cmpstr(ps, ==, "foo");
    std::string ss = variant_to_string(child);
    g_assert_cmpstr(ss.c_str(), ==, "foo");
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/variant-utils/string-roundtrip", test_string_roundtrip);
    g_test_add_func("/variant-utils/empty-string",     test_empty_string);
    g_test_add_func("/variant-utils/child-value",      test_child_value);
    return g_test_run();
}

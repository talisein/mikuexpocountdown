#include <iostream>
#include <ranges>
#include <sstream>
#include <format>
#include "search_provider.h"
#include "events.h"
#include "util.hpp"

PEEL_CLASS_IMPL(SearchProvider, "MikuSearchProvider", peel::Gio::DBusInterfaceSkeleton)

peel::Signal<SearchProvider, void(const char *)> SearchProvider::s_activate;

void SearchProvider::Class::init() {
    override_vfunc_dispose<SearchProvider>();
    override_vfunc_get_info<SearchProvider>();
    override_vfunc_get_vtable<SearchProvider>();
    s_activate = peel::Signal<SearchProvider, void(const char *)>::create("activate");
}

void SearchProvider::vfunc_dispose() {
    if (get_connections())
        unexport();
    parent_vfunc_dispose<SearchProvider>();
}

static const char interfaceXml0[] = R"XML_DELIMITER(<!DOCTYPE node PUBLIC
'-//freedesktop//DTD D-BUS Object Introspection 1.0//EN'
'http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd'>
<node>
  <interface name="org.gnome.Shell.SearchProvider2">
    <method name="GetInitialResultSet">
      <arg type="as" name="terms" direction="in" />
      <arg type="as" name="results" direction="out" />
    </method>
    <method name="GetSubsearchResultSet">
      <arg type="as" name="previous_results" direction="in" />
      <arg type="as" name="terms" direction="in" />
      <arg type="as" name="results" direction="out" />
    </method>
    <method name="GetResultMetas">
      <arg type="as" name="identifiers" direction="in" />
      <arg type="aa{sv}" name="metas" direction="out" />
    </method>
    <method name="ActivateResult">
      <arg type="s" name="identifier" direction="in" />
      <arg type="as" name="terms" direction="in" />
      <arg type="u" name="timestamp" direction="in" />
    </method>
    <method name="LaunchSearch">
      <arg type="as" name="terms" direction="in" />
      <arg type="u" name="timestamp" direction="in" />
    </method>
  </interface>
</node>
)XML_DELIMITER";

peel::Gio::DBusInterface::Info *SearchProvider::vfunc_get_info() {
    static GDBusNodeInfo *node = g_dbus_node_info_new_for_xml(interfaceXml0, nullptr);
    return reinterpret_cast<peel::Gio::DBusInterface::Info *>(
        g_dbus_node_info_lookup_interface(node, "org.gnome.Shell.SearchProvider2"));
}

namespace {

std::vector<peel::String> parse_string_array(GVariant *as_var) {
    std::vector<peel::String> result;
    GVariantIter *iter;
    g_variant_get(as_var, "as", &iter);
    const char *s;
    while (g_variant_iter_loop(iter, "s", &s))
        result.push_back(copy_to_peel_string(std::string_view{s}));
    g_variant_iter_free(iter);
    return result;
}

GVariant *build_search_results(const std::vector<peel::String> &terms) {
    using namespace std::ranges;
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
    for (const auto &e : Miku::get_events()
                         | views::filter(&Miku::Event::is_unexpired)
                         | views::filter([&terms](const auto &ev) { return ev->match_search(terms); }))
        g_variant_builder_add(&builder, "s", (const char *)e->name());
    return g_variant_new("(@as)", g_variant_builder_end(&builder));
}

void return_as(GDBusMethodInvocation *inv, GVariant *as_result) {
    g_dbus_method_invocation_return_value(inv, as_result);
}

void return_empty(GDBusMethodInvocation *inv) {
    g_dbus_method_invocation_return_value(inv, g_variant_new_tuple(nullptr, 0));
}

}

peel::Gio::DBusInterface::VTable *SearchProvider::vfunc_get_vtable() {
    static const GDBusInterfaceVTable vtable = {
        +[](GDBusConnection *, const char *, const char *, const char *,
            const char *method_name, GVariant *params,
            GDBusMethodInvocation *inv, void *user_data) {
            auto *self = static_cast<SearchProvider *>(user_data);

            if (strcmp(method_name, "GetInitialResultSet") == 0) {
                GVariant *as_var = g_variant_get_child_value(params, 0);
                auto terms = parse_string_array(as_var);
                g_variant_unref(as_var);
                return_as(inv, build_search_results(terms));

            } else if (strcmp(method_name, "GetSubsearchResultSet") == 0) {
                GVariant *as_var = g_variant_get_child_value(params, 1);
                auto terms = parse_string_array(as_var);
                g_variant_unref(as_var);
                return_as(inv, build_search_results(terms));

            } else if (strcmp(method_name, "GetResultMetas") == 0) {
                GVariant *as_var = g_variant_get_child_value(params, 0);
                auto identifiers = parse_string_array(as_var);
                g_variant_unref(as_var);

                GVariantBuilder outer;
                g_variant_builder_init(&outer, G_VARIANT_TYPE("aa{sv}"));
                static constexpr char date_format[] { "{0:%A} {0:%d} {0:%B} {0:%Y} {0:%X} {0:%Z}" };

                for (const auto &id : identifiers) {
                    auto it = std::ranges::find_if(Miku::get_events(),
                        [&id](const peel::RefPtr<Miku::Event> &e) {
                            return to_sview(e->name()) == to_sview(id);
                        });
                    if (it == std::end(Miku::get_events())) continue;
                    const auto &ev = *it;

                    auto diff_secs = ev->secs_to_live();
                    auto diff_days = std::chrono::floor<std::chrono::days>(diff_secs);
                    std::ostringstream ss;
                    using namespace std::chrono_literals;
                    if (diff_days.count() > 0) {
                        ss << "in about " << diff_days.count() << " day";
                        if (diff_days.count() > 1) ss << 's';
                    } else {
                        auto diff_hours = std::chrono::floor<std::chrono::hours>(diff_secs);
                        if (diff_hours.count() > 0) {
                            ss << "in about " << diff_hours.count() << " hour";
                            if (diff_hours.count() > 1) ss << 's';
                        } else {
                            auto diff_minutes = std::chrono::floor<std::chrono::minutes>(diff_secs);
                            if (diff_minutes.count() < 1) {
                                if (diff_secs.count() <= 0) {
                                    if (!ev->is_expired()) ss << "Happening now";
                                    else                   ss << "Event finished";
                                } else {
                                    ss << "in less than a minute!";
                                }
                            } else if (diff_minutes.count() == 1) {
                                ss << "in a minute!";
                            } else {
                                ss << "in about " << diff_minutes.count() << " minutes";
                            }
                        }
                    }

                    std::string display_date;
                    if (diff_secs.count() <= 0) {
                        auto end_time = ev->end_time();
                        if (!ev->is_expired())
                            display_date = std::format("until {}", std::format(date_format, std::chrono::zoned_time(std::chrono::current_zone(), end_time)));
                        else
                            display_date = std::format("ended {}", std::format(date_format, std::chrono::zoned_time(std::chrono::current_zone(), end_time)));
                    } else {
                        display_date = std::format(date_format, std::chrono::zoned_time(std::chrono::current_zone(), ev->start_time()));
                    }
                    auto description = format_to_peel("{} — {}", ss.str(), display_date);

                    GVariantBuilder dict;
                    g_variant_builder_init(&dict, G_VARIANT_TYPE("a{sv}"));
                    g_variant_builder_add(&dict, "{sv}", "id",          g_variant_new_string((const char *)id));
                    g_variant_builder_add(&dict, "{sv}", "name",        g_variant_new_string((const char *)ev->name()));
                    g_variant_builder_add(&dict, "{sv}", "gicon",       g_variant_new_string("dance._39music.MikuExpoCountdown"));
                    g_variant_builder_add(&dict, "{sv}", "description", g_variant_new_string(description));
                    g_variant_builder_add_value(&outer, g_variant_builder_end(&dict));
                }
                g_dbus_method_invocation_return_value(inv,
                    g_variant_new("(@aa{sv})", g_variant_builder_end(&outer)));

            } else if (strcmp(method_name, "ActivateResult") == 0) {
                const char *identifier;
                g_variant_get_child(params, 0, "&s", &identifier);
                s_activate.emit(self, identifier);
                return_empty(inv);

            } else if (strcmp(method_name, "LaunchSearch") == 0) {
                return_empty(inv);
            }
        },
        nullptr, nullptr
    };
    return reinterpret_cast<peel::Gio::DBusInterface::VTable *>(
        const_cast<GDBusInterfaceVTable *>(&vtable));
}

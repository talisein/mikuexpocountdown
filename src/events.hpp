#pragma once
#include <algorithm>
#include <chrono>
#include <vector>
#include <ranges>
#include <peel/class.h>
#include <peel/GObject/Object.h>
#include <peel/String.h>
#include <peel/GLib/functions.h>
#include "util.hpp"

using namespace std::chrono;

namespace Miku
{
    struct raw_event;

    class Event final : public peel::GObject::Object
    {
        PEEL_SIMPLE_CLASS(Event, peel::GObject::Object);

        struct Members {
            peel::String name;
            peel::String style_class;
            peel::String search_keys;
            zoned_seconds start_time;
            zoned_seconds end_time;
            std::chrono::minutes duration;
        } m;

        void init(Class *);
        void vfunc_dispose();

        int64_t get_start_time_unix() const {
            return static_cast<int64_t>(m.start_time.get_sys_time().time_since_epoch().count());
        }
        int64_t get_end_time_unix() const {
            return static_cast<int64_t>(m.end_time.get_sys_time().time_since_epoch().count());
        }
        const char* get_timezone_name() const {
            return m.start_time.get_time_zone()->name().data();
        }

        template<typename F>
        static void define_properties(F &f) {
            f.prop(prop_name(),          (const char*)nullptr).get(&Event::name);
            f.prop(prop_style_class(),   (const char*)nullptr).get(&Event::style_class);
            f.prop(prop_start_time(),    G_MININT64, G_MAXINT64, int64_t(0)).get(&Event::get_start_time_unix);
            f.prop(prop_end_time(),      G_MININT64, G_MAXINT64, int64_t(0)).get(&Event::get_end_time_unix);
            f.prop(prop_timezone_name(), (const char*)nullptr).get(&Event::get_timezone_name);
        }

    public:
        static peel::RefPtr<Event> create(const raw_event &event);

        PEEL_PROPERTY(const char*, name,          "name")
        PEEL_PROPERTY(const char*, style_class,   "style-class")
        PEEL_PROPERTY(int64_t,     start_time,    "start-time")
        PEEL_PROPERTY(int64_t,     end_time,      "end-time")
        PEEL_PROPERTY(const char*, timezone_name, "timezone-name")

        const peel::String& name() const       { return m.name; }
        const peel::String& style_class() const { return m.style_class; }
        const zoned_seconds& start_time() const { return m.start_time; }
        const zoned_seconds& end_time() const   { return m.end_time; }

        bool is_live() const {
            using namespace std::chrono_literals;
            return 0s > secs_to_live();
        }

        bool is_expired() const {
            using namespace std::chrono_literals;
            return 0s > secs_to_expire();
        }

        bool is_unexpired() const { return !is_expired(); }

        std::chrono::seconds secs_to_live() const {
            return floor<std::chrono::seconds>(m.start_time.get_sys_time()
                                               - std::chrono::system_clock::now());
        }

        std::chrono::seconds secs_to_expire() const {
            return secs_to_live() + m.duration;
        }

        bool match_search(const std::vector<peel::String> &terms) const {
            using namespace std::ranges;
            const auto match_keys = [this](const peel::String &term) -> bool {
                auto cf = peel::GLib::utf8_casefold(term, -1);
                return to_sview(m.search_keys).find(to_sview(cf)) != std::string_view::npos;
            };
            return std::ranges::all_of(terms, match_keys);
        }
    };

    using events_view_t = std::ranges::ref_view<const std::vector<peel::RefPtr<Event>>>;
    events_view_t get_events();
}

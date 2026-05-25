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
            peel::String m_style_class;
            peel::String search_keys;
            zoned_seconds start_time;
            zoned_seconds end_time;
            std::chrono::minutes m_duration;
        } *m = nullptr;

        void init(Class *) {}
        void vfunc_dispose();
        ~Event() { delete m; }

    public:
        static peel::RefPtr<Event> create(const raw_event &event);

        const peel::String& name() const       { return m->name; }
        const peel::String& style_class() const { return m->m_style_class; }
        const zoned_seconds& start_time() const { return m->start_time; }
        const zoned_seconds& end_time() const   { return m->end_time; }

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
            return floor<std::chrono::seconds>(m->start_time.get_sys_time()
                                               - std::chrono::system_clock::now());
        }

        std::chrono::seconds secs_to_expire() const {
            return secs_to_live() + m->m_duration;
        }

        bool match_search(const std::vector<peel::String> &terms) const {
            using namespace std::ranges;
            const auto match_keys = [this](const peel::String &term) -> bool {
                auto cf = peel::GLib::utf8_casefold(term, -1);
                return to_sview(m->search_keys).find(to_sview(cf)) != std::string_view::npos;
            };
            return std::ranges::all_of(terms, match_keys);
        }
    };

    using events_view_t = std::ranges::ref_view<const std::vector<peel::RefPtr<Event>>>;
    events_view_t get_events();
}

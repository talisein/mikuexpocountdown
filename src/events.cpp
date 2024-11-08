#include <iostream>
#include <ranges>
#include "events.h"

namespace Miku {
    using namespace std::chrono_literals;

   struct raw_event {
        std::string_view name;
        std::string_view search_terms;
        std::string_view style_class;
        local_seconds time;
        std::chrono::minutes duration;
        time_zone const * const tz;
    };
}

namespace {
    using namespace std::chrono_literals;

    static time_zone const * const JST { locate_zone("Asia/Tokyo") };
    static time_zone const * const VANCOUVER_TZ { locate_zone("America/Vancouver") };
    static time_zone const * const PST { locate_zone("America/Los_Angeles") };
    static time_zone const * const PHOENIX_TZ { locate_zone("America/Phoenix") };
    static time_zone const * const MST { locate_zone("America/Denver") };
    static time_zone const * const CST { locate_zone("America/Chicago") };
    static time_zone const * const EST { locate_zone("America/New_York") };
    static time_zone const * const DETROIT_TZ { locate_zone("America/Detroit") };
    static time_zone const * const TORONTO_TZ { locate_zone("America/Toronto") };
    static time_zone const * const MEXICO_CITY_TZ { locate_zone("America/Mexico_City") };

    const std::array raw_events {std::to_array<Miku::raw_event>({
                {"Miku's Birthday"   , "Miku's Birthday",     "birthday",   local_days{August/31/2025},        24h, JST},
            })};

}

namespace Miku
{
    events_view_t get_events()
    {
        static constinit auto events_view { std::views::transform(raw_events,
                                                                  []<typename T>(T&& raw) { return Miku::Event::create(std::forward<T>(raw)); }) };
        static const std::vector<Glib::RefPtr<const Miku::Event>> events {events_view.begin(), events_view.end()};
        return std::ranges::views::all(events);
    }

    Event::Event(const raw_event &event) :
        Glib::ObjectBase("event"),
        name(event.name.begin(), event.name.end()),
        m_style_class(event.style_class.begin(), event.style_class.end()),
        search_keys(Glib::ustring(event.search_terms.begin(), event.search_terms.end()).casefold()),
        start_time(event.tz, event.time),
        end_time(event.tz, event.time + event.duration),
        m_duration(event.duration),
        m_local_time(event.time)
    {
    }
}

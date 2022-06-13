#include <iostream>
#include <ranges>
#include "events.h"

namespace Miku {
   struct raw_event {
        std::string_view name;
        std::string_view search_terms;
        std::string_view style_class;
        date::local_seconds time;
        std::chrono::minutes duration;
    };
}

namespace {
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono_literals;

    constinit std::array raw_events {std::to_array<Miku::raw_event>({
                {"Mirai Tix",     "miku Magical Mirai Tickets tix",       "mirai",   local_days{June/15/2022}     + 12h, 24h * 5 + 12h},
                {"DJ Bar 39",     "miku DJ Bar 39 mogra",                 "bar39",   local_days{June/15/2022}     + 18h, 4h},
                {"Dimension 39",  "miku Another Dimension 39",            "d39",     local_days{July/9/2022}      + 23h, 4h},
                {"Mirai Osaka",   "miku Magical Mirai Osaka",             "mirai",   local_days{August/12/2022}   + 12h, 48h + 12h},
                {"Mirai Tokyo",   "miku Magical Mirai Tokyo",             "mirai",   local_days{September/2/2022} + 12h, 48h + 12h},
                {"Mirai Sapporo", "miku Magical Mirai Sapporo Snow Miku", "mirai",   local_days{February/4/2023}  + 12h, 48h},
            })};

    constinit auto events_view { std::views::transform(raw_events, &Miku::Event::create) };

    static date::time_zone const * const JST { date::locate_zone("Asia/Tokyo") };
}


namespace Miku
{
    const std::vector<Glib::RefPtr<const Miku::Event>>& get_events() {
        static std::vector<Glib::RefPtr<const Miku::Event>> events {events_view.begin(), events_view.end()};
        return events;
    }

    Glib::RefPtr<Event>
    Event::create(const raw_event& event)
    {
        return Glib::RefPtr<Miku::Event>(new Miku::Event(event));
    }

    Event::Event(const raw_event &event) :
        Glib::ObjectBase("event"),
        name(event.name.begin(), event.name.end()),
        m_style_class(event.style_class.begin(), event.style_class.end()),
        search_keys(Glib::ustring(event.search_terms.begin(), event.search_terms.end()).casefold()),
        start_time(JST, event.time),
        end_time(JST, event.time + event.duration),
        m_duration(event.duration), m_local_time(event.time)
    {
    }
}

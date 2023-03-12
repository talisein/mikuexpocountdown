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

    constexpr std::array raw_events {std::to_array<Miku::raw_event>({
                {"Thunderbolt Sapporo",     "miku Thunderbolt",       "thunderbolt",   local_days{March/25/2023}     + 18h, 2h},
                {"Kodo Miku",   "miku Kodo NHK",             "kodo",   local_days{June/3/2023}   + 13h, 24h + 6h},
                {"Mirai Osaka",   "miku Magical Mirai Osaka",             "mirai",   local_days{August/11/2023}   + 12h, 48h + 12h},
                {"Mirai Tokyo",   "miku Magical Mirai Tokyo",             "mirai",   local_days{September/3/2023} + 12h, 48h + 12h},
            })};

    static date::time_zone const * const JST { date::locate_zone("Asia/Tokyo") };
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
        start_time(JST, event.time),
        end_time(JST, event.time + event.duration),
        m_duration(event.duration),
        m_local_time(event.time)
    {
    }
}

#include <iostream>
#include <ranges>
#include "events.h"

namespace Miku {
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono_literals;

   struct raw_event {
        std::string_view name;
        std::string_view search_terms;
        std::string_view style_class;
        date::local_seconds time;
        std::chrono::minutes duration;
       date::time_zone const * const tz;
    };
}

namespace {
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono_literals;

    static date::time_zone const * const JST { date::locate_zone("Asia/Tokyo") };
    static date::time_zone const * const VANCOUVER_TZ { date::locate_zone("America/Vancouver") };
    static date::time_zone const * const PST { date::locate_zone("America/Los_Angeles") };
    static date::time_zone const * const PHOENIX_TZ { date::locate_zone("America/Phoenix") };
    static date::time_zone const * const MST { date::locate_zone("America/Denver") };
    static date::time_zone const * const CST { date::locate_zone("America/Chicago") };
    static date::time_zone const * const EST { date::locate_zone("America/New_York") };
    static date::time_zone const * const DETROIT_TZ { date::locate_zone("America/Detroit") };
    static date::time_zone const * const TORONTO_TZ { date::locate_zone("America/Toronto") };
    static date::time_zone const * const MEXICO_CITY_TZ { date::locate_zone("America/Mexico_City") };

    const std::array raw_events {std::to_array<Miku::raw_event>({
                {"VR",          "Miku Expo VR",        "expovr",   local_days{November/11/2023} + 18h, 2h, PST},
                {"Vancouver"  , "Miku Expo Vancouver", "expo2024", local_days{April/4/2024}     + 20h, 2h, VANCOUVER_TZ},
                {"Portland"   , "Miku Expo Portland",  "expo2024", local_days{April/6/2024}     + 20h, 2h, PST},
                {"San Jose 1" , "Miku Expo San Jose",  "expo2024", local_days{April/8/2024}     + 20h, 2h, PST},
                {"San Jose 2" , "Miku Expo San Jose",  "expo2024", local_days{April/9/2024}     + 20h, 2h, PST},
                {"Phoenix"    , "Miku Expo Phoenix",   "expo2024", local_days{April/14/2024}    + 20h, 2h, PHOENIX_TZ},
                {"Denver"     , "Miku Expo Denver",    "expo2024", local_days{April/22/2024}    + 20h, 2h, MST},
                {"Dallas"     , "Miku Expo Dallas",    "expo2024", local_days{April/25/2024}    + 20h, 2h, CST},
                {"Austin"     , "Miku Expo Austin",    "expo2024", local_days{April/27/2024}    + 20h, 2h, CST},
                {"Atlanta"    , "Miku Expo Atlanta",   "expo2024", local_days{April/30/2024}    + 20h, 2h, EST},
                {"Orlando"    , "Miku Expo Orlando",   "expo2024", local_days{May/2/2024}       + 20h, 2h, EST},
                {"Washington DC", "Miku Expo DC",      "expo2024", local_days{May/5/2024}       + 20h, 2h, EST},
                {"Newark"     , "Miku Expo Newark",    "expo2024", local_days{May/7/2024}       + 20h, 2h, EST},
                {"Boston"     , "Miku Expo Boston",    "expo2024", local_days{May/9/2024}       + 20h, 2h, EST},
                {"Detroit"    , "Miku Expo Detroit",   "expo2024", local_days{May/12/2024}      + 20h, 2h, DETROIT_TZ},
                {"Chicago"    , "Miku Expo Chicago",   "expo2024", local_days{May/14/2024}      + 20h, 2h, CST},
                {"Toronto"    , "Miku Expo Toronto",   "expo2024", local_days{May/16/2024}      + 20h, 2h, TORONTO_TZ},
                {"Mexico City", "Miku Expo Mexico",    "expo2024", local_days{May/21/2024}      + 20h, 2h, MEXICO_CITY_TZ},
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

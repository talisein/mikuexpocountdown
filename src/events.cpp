#include <generator>
#include <iostream>
#include <ranges>
#include <mutex>
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
    static time_zone const * const AUCKLANDT { locate_zone("Pacific/Auckland") };
    static time_zone const * const BRISBANET { locate_zone("Australia/Brisbane") };
    static time_zone const * const SYDNEYT { locate_zone("Australia/Sydney") };
    static time_zone const * const MELBOURNET { locate_zone("Australia/Melbourne") };
    static time_zone const * const PERTHT { locate_zone("Australia/Perth") };

    const std::array raw_events {std::to_array<Miku::raw_event>({
                {"Blooming Osaka",  "Hatsune Miku Japan Tour Blooming Osaka",  "blooming", local_days{April/19/2025} + 13h, 2h, JST},
                {"Blooming Aichi",  "Hatsune Miku Japan Tour Blooming Aichi Nagoya",  "blooming", local_days{April/26/2025} + 13h, 2h, JST},
                {"Blooming Fukuoka",  "Hatsune Miku Japan Tour Blooming Fukuoka",  "blooming", local_days{April/29/2025} + 13h, 2h, JST},
                {"Blooming Tokyo Day 1",  "Hatsune Miku Japan Tour Blooming Tokyo",  "blooming", local_days{May/2/2025} + 13h, 2h, JST},
                {"Blooming Tokyo Day 2",  "Hatsune Miku Japan Tour Blooming Tokyo",  "blooming", local_days{May/3/2025} + 13h, 2h, JST},
                {"Blooming Kagawa",  "Hatsune Miku Japan Tour Blooming Kagawa",  "blooming", local_days{May/6/2025} + 13h, 2h, JST},
                {"Blooming Hokkaido",  "Hatsune Miku Japan Tour Blooming Hokkaido Sapporo",  "blooming", local_days{May/17/2025} + 18h, 2h, JST},
                {"Blooming Okinawa",  "Hatsune Miku Japan Tour Blooming Okinawa Naha",  "blooming", local_days{May/31/2025} + 13h, 2h, JST},
            })};

}

namespace Miku
{
    std::vector<Glib::RefPtr<const Miku::Event>> create_events() {
        std::vector<Glib::RefPtr<const Miku::Event>> events;
        events.reserve(raw_events.size() + 1);
        for (const raw_event& e : raw_events) {
            events.push_back(Miku::Event::create(e));
        }

        auto this_year = std::chrono::year_month_day(std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())).year(), August, std::chrono::day(31));
        auto next_year = std::chrono::year_month_day(this_year.year() + std::chrono::years(1), August, std::chrono::day(31));
        raw_event next_birthday {"Miku's Birthday"   , "Hatsune Miku Birthday",     "birthday",
            std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) < this_year ? local_days(this_year) : local_days(next_year),
            24h, JST};
        events.push_back(Miku::Event::create(next_birthday));

        return events;
    }

    events_view_t get_events()
    {
        static const std::vector<Glib::RefPtr<const Miku::Event>> events { create_events() };
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

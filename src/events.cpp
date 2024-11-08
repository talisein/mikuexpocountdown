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
                {"Miku Expo Auckland"   , "Miku Expo Auckland New Zealand Australia",     "nzaus",   local_days{November/15/2024} + 12h + 6h, 2h, AUCKLANDT},
                {"Miku Expo Brisbane"   , "Miku Expo Auckland New Zealand Australia",     "nzaus",   local_days{November/18/2024} + 12h + 7h, 2h, BRISBANET},
                {"Miku Expo Sydney"     , "Miku Expo Auckland New Zealand Australia",     "nzaus",   local_days{November/18/2024} + 12h + 8h, 2h, SYDNEYT},
                {"Miku Expo Melbourne"  , "Miku Expo Auckland New Zealand Australia",     "nzaus",   local_days{November/18/2024} + 12h + 7h, 2h, MELBOURNET},
                {"Miku Expo Perth"      , "Miku Expo Auckland New Zealand Australia",     "nzaus",   local_days{November/18/2024} + 12h + 8h, 2h, PERTHT},
            })};

}

namespace Miku
{
    std::generator<Glib::RefPtr<Miku::Event>> event_gen() {
        for (const raw_event& e : raw_events) {
            co_yield Miku::Event::create(e);
        }
        auto this_year = std::chrono::year_month_day(std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())).year(), August, std::chrono::day(31));
        auto next_year = std::chrono::year_month_day(this_year.year() + std::chrono::years(1), August, std::chrono::day(31));
        raw_event next_birthday {"Miku's Birthday"   , "Miku's Birthday",     "birthday",
            std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) < this_year ? local_days(this_year) : local_days(next_year),
            24h, JST};
        co_yield Miku::Event::create(next_birthday);
    }

    events_view_t get_events()
    {
        static std::vector<Glib::RefPtr<const Miku::Event>> events;
        std::once_flag flag;
        std::call_once(flag, [] {
            events.reserve(raw_events.size() + 1);
            for (auto e : event_gen()) {
                events.push_back(e);
            }
        });
        return std::ranges::views::all(std::as_const(events));
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

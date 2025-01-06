#include <generator>
#include <iostream>
#include <ranges>
#include <mutex>
#include "events.h"
#include "config.h"

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

#if IS_IANA_BUNDLED
namespace __gnu_cxx
{
    const char* zoneinfo_dir_override()
    {
        return BUNDLED_IANA_DIR.data();
    }
}
#endif

namespace {
    using namespace std::chrono_literals;

    static const struct tz_class {
        tz_class() {
            try {
                JST = locate_zone("Asia/Tokyo");
                VANCOUVER_TZ = locate_zone("America/Vancouver");
                PST =  locate_zone("America/Los_Angeles");
                PHOENIX_TZ =  locate_zone("America/Phoenix");
                MST =  locate_zone("America/Denver");
                CST =  locate_zone("America/Chicago");
                EST =  locate_zone("America/New_York");
                DETROIT_TZ =  locate_zone("America/Detroit");
                TORONTO_TZ =  locate_zone("America/Toronto");
                MEXICO_CITY_TZ =  locate_zone("America/Mexico_City");
                AUCKLANDT =  locate_zone("Pacific/Auckland");
                BRISBANET =  locate_zone("Australia/Brisbane");
                SYDNEYT =  locate_zone("Australia/Sydney");
                MELBOURNET =  locate_zone("Australia/Melbourne");
                PERTHT =  locate_zone("Australia/Perth");
            } catch (std::runtime_error &e) {
                std::cerr << "Failed to load required timezones. This is likely due to an incompatibility between the latest tzdb 20204b and GCC 14s stdlib, see bug 116657.\nSpecific error: " << e.what() << "\nTerminating, because the purpose of this program is to translate timezones.\nYou can rebuild this program with the 'bundle_iana_tzdata' option to get a working version based on the 2024a dataset.\n";
                exit(1);
            }
        };

        time_zone const * JST;
        time_zone const * VANCOUVER_TZ;
        time_zone const * PST;
        time_zone const * PHOENIX_TZ;
        time_zone const * MST;
        time_zone const * CST;
        time_zone const * EST;
        time_zone const * DETROIT_TZ;
        time_zone const * TORONTO_TZ;
        time_zone const * MEXICO_CITY_TZ;
        time_zone const * AUCKLANDT;
        time_zone const * BRISBANET;
        time_zone const * SYDNEYT;
        time_zone const * MELBOURNET;
        time_zone const * PERTHT;
    } TZ {};

    const std::array raw_events {std::to_array<Miku::raw_event>({
                {"Blooming Osaka",  "Hatsune Miku Japan Tour Blooming Osaka",  "blooming", local_days{April/19/2025} + 13h, 2h, TZ.JST},
                {"Blooming Aichi",  "Hatsune Miku Japan Tour Blooming Aichi Nagoya",  "blooming", local_days{April/26/2025} + 13h, 2h, TZ.JST},
                {"Blooming Fukuoka",  "Hatsune Miku Japan Tour Blooming Fukuoka",  "blooming", local_days{April/29/2025} + 13h, 2h, TZ.JST},
                {"Blooming Tokyo Day 1",  "Hatsune Miku Japan Tour Blooming Tokyo",  "blooming", local_days{May/2/2025} + 13h, 2h, TZ.JST},
                {"Blooming Tokyo Day 2",  "Hatsune Miku Japan Tour Blooming Tokyo",  "blooming", local_days{May/3/2025} + 13h, 2h, TZ.JST},
                {"Blooming Kagawa",  "Hatsune Miku Japan Tour Blooming Kagawa",  "blooming", local_days{May/6/2025} + 13h, 2h, TZ.JST},
                {"Blooming Hokkaido",  "Hatsune Miku Japan Tour Blooming Hokkaido Sapporo",  "blooming", local_days{May/17/2025} + 18h, 2h, TZ.JST},
                {"Blooming Okinawa",  "Hatsune Miku Japan Tour Blooming Okinawa Naha",  "blooming", local_days{May/31/2025} + 13h, 2h, TZ.JST},
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
            24h, TZ.JST};
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

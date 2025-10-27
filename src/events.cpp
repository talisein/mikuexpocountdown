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
                DENVER_TZ = locate_zone("America/Denver");
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
                BANGKOKT = locate_zone("Asia/Bangkok");
                HONGKONGT = locate_zone("Asia/Hong_Kong");
                JAKARTAT = locate_zone("Asia/Jakarta");
                MANILAT = locate_zone("Asia/Manila");
                SINGAPORET = locate_zone("Asia/Singapore");
                KUALALUMPURT = locate_zone("Asia/Kuala_Lumpur");
                TAIPEIT = locate_zone("Asia/Taipei");
                SEOULT = locate_zone("Asia/Seoul");
                CHICAGOT = locate_zone("America/Chicago");
                ARIZONAT = locate_zone("US/Arizona");
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
        time_zone const * DENVER_TZ;
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
        time_zone const * BANGKOKT;
        time_zone const * HONGKONGT;
        time_zone const * JAKARTAT;
        time_zone const * MANILAT;
        time_zone const * SINGAPORET;
        time_zone const * KUALALUMPURT;
        time_zone const * TAIPEIT;
        time_zone const * SEOULT;
        time_zone const * CHICAGOT;
        time_zone const * ARIZONAT;
    } TZ {};

    const std::array raw_events {std::to_array<Miku::raw_event>({
                {"Miku Expo Bangkok",  "Hatsune Miku Miku Expo Asia Bangkok Thailand",  "expoasia2025", local_days{November/5/2025} + 20h, 3h, TZ.BANGKOKT},
                {"Miku Expo Hong Kong",  "Hatsune Miku Miku Expo Asia Hong Kong",  "expoasia2025", local_days{November/8/2025} + 20h, 3h, TZ.HONGKONGT},
                {"Miku Expo Jakarta",  "Hatsune Miku Miku Expo Asia Jakarta Indonesia",  "expoasia2025", local_days{November/12/2025} + 20h, 3h, TZ.JAKARTAT},
                {"Miku Expo Manila",  "Hatsune Miku Miku Expo Asia Manila Philippines",  "expoasia2025", local_days{November/16/2025} + 20h, 3h, TZ.MANILAT},
                {"Miku Expo Singapore",  "Hatsune Miku Miku Expo Singapore",  "expoasia2025", local_days{November/19/2025} + 20h, 3h, TZ.SINGAPORET},
                {"Miku Expo Kuala Lumpur",  "Hatsune Miku Miku Expo Asia Kuala Lumpur Malaysia",  "expoasia2025", local_days{November/22/2025} + 20h, 3h, TZ.KUALALUMPURT},
                {"Miku Expo Taipei",  "Hatsune Miku Miku Expo Asia Taipei Taiwan",  "expoasia2025", local_days{November/26/2025} + 20h, 3h, TZ.TAIPEIT},
                {"Miku Expo Seoul",  "Hatsune Miku Miku Expo Asia Seoul Korea",  "expoasia2025", local_days{November/29/2025} + 20h, 3h + 24h, TZ.SEOULT},
                {"Miku Expo Chicago",  "Hatsune Miku Miku Expo NA North America Chicago",  "expona2026", local_days{April/13/2026} + 20h, 3h, TZ.CHICAGOT},
                {"Miku Expo Denver",  "Hatsune Miku Miku Expo NA North America Denver",  "expona2026", local_days{April/15/2026} + 20h, 3h, TZ.DENVER_TZ},
                {"Miku Expo Vancouver",  "Hatsune Miku Miku Expo NA North America Vancouver Canada",  "expona2026", local_days{April/18/2026} + 20h, 3h, TZ.VANCOUVER_TZ},
                {"Miku Expo Seattle",  "Hatsune Miku Miku Expo NA North America ",  "expona2026", local_days{April/20/2026} + 20h, 3h, TZ.PST},
                {"Miku Expo San Jose",  "Hatsune Miku Miku Expo NA North America San Jose California",  "expona2026", local_days{April/22/2026} + 20h, 3h, TZ.PST},
                {"Miku Expo Los Angeles",  "Hatsune Miku Miku Expo NA North America Los Angeles California",  "expona2026", local_days{April/25/2026} + 20h, 3h, TZ.PST},
                {"Miku Expo Glendale AZ",  "Hatsune Miku Miku Expo NA North America Glendale Arizona",  "expona2026", local_days{April/28/2026} + 20h, 3h, TZ.ARIZONAT},
                {"Miku Expo Grand Praire TX",  "Hatsune Miku Miku Expo NA North America Grand Praire Texas",  "expona2026", local_days{April/30/2026} + 20h, 3h, TZ.CST},
                {"Miku Expo Cedar Park TX",  "Hatsune Miku Miku Expo NA North America Cedar Park Texas",  "expona2026", local_days{May/1/2026} + 20h, 3h, TZ.CST},
                {"Miku Expo Duluth GA",  "Hatsune Miku Miku Expo NA North America Duluth Georgia",  "expona2026", local_days{May/3/2026} + 19h, 3h, TZ.EST},
                {"Miku Expo Washington DC",  "Hatsune Miku Miku Expo NA North America Washington DC",  "expona2026", local_days{May/5/2026} + 20h, 3h, TZ.EST},
                {"Miku Expo Newark NJ",  "Hatsune Miku Miku Expo NA North America Newark New Jersey",  "expona2026", local_days{May/7/2026} + 20h, 3h, TZ.EST},
                {"Miku Expo Boston",  "Hatsune Miku Miku Expo NA North America Boston",  "expona2026", local_days{May/10/2026} + 20h, 3h, TZ.EST},
                {"Miku Expo Hamilton ON",  "Hatsune Miku Miku Expo NA North America Hamilton Ontario Canada",  "expona2026", local_days{May/13/2026} + 20h, 3h, TZ.TORONTO_TZ},
                {"Miku Expo Mexico City",  "Hatsune Miku Miku Expo NA North America Mexico City",  "expona2026", local_days{May/19/2026} + 21h, 3h, TZ.MEXICO_CITY_TZ},
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

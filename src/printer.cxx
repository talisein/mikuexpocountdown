#include <chrono>
#include <iostream>
#include <iomanip>
#include <array>
#include <ranges>
#include <experimental/iterator>
#include "date/date.h"
#include "date/tz.h"

namespace {
    using namespace date;
    using namespace std::chrono_literals;

    struct event {
        std::string_view name;
        date::local_seconds date;
        std::chrono::minutes duration;
    };

    constinit std::array<const event, 2> events {{
            {"Miku Expo Rewind", local_days{June/5/2022} + 0h, 3h},
            {"Digital Stars 2022", local_days{May/28/2022} + 14h, 6h},
        }};

    struct offset {
        std::string_view DJ;
        std::chrono::minutes offset;
    };

    constexpr std::array<offset, 9> offsets = {{
        {"Opening Act", 0min},
        {"Jamie Paige", 50min},
        {"Mwk", 1h + 20min},
        {"Dubscribe", 1h + 50min},
        {"Amamori P", 2h + 25min},
        {"Amenkensetsu", 2h + 55min},
        {"DJ Shimamura", 3h + 30min},
        {"Pedestrian", 4h},
        {"monaca:factory", 4h + 30min},
    }};

    const std::array<const date::time_zone*, 5> zones = {
            date::locate_zone("America/Los_Angeles"),
//            date::locate_zone("America/Denver"),
//            date::locate_zone("America/Chicago"),
            date::locate_zone("America/New_York"),
            date::locate_zone("Europe/London"),
            date::locate_zone("Europe/Paris"),
            date::locate_zone("Asia/Tokyo")
    };
}

std::string
url_encode(const std::string_view sv) {
    std::stringstream out;
    for (auto c : sv) {
        if (c == ' ')
            out << "%20";
        else
            out << c;
    }

    return out.str();
}

int main() {
    auto joiner = std::experimental::make_ostream_joiner(std::cout, "|");
    auto radio_formatter = [](auto x){return date::format("%A %b&nbsp;%d, %I:%M&nbsp;%p", x);};
//    auto radio_formatter = [](auto x){return date::format("%I:%M&nbsp;%p", x);};
    auto gcal_formatter = [](auto x){return date::format("%Y%m%dT%H%M00Z", x);};

    for (const auto & event : events) {
        std::stringstream header;

        const auto jp_time { date::make_zoned("Asia/Tokyo", event.date) };
        const date::sys_seconds sys_time { jp_time.get_sys_time() };
        header << event.name << " is " << jp_time << '\n';
        header << "Google Calendar event: ";
        header << "https://calendar.google.com/calendar/u/0/r/eventedit?text="
               << url_encode(event.name)
               << "&dates="
               << gcal_formatter(sys_time)
               << '/'
               << gcal_formatter(sys_time + event.duration)
               << "&ctz=Asia%2FTokyo"
               << '\n';

        header << '\n';

        joiner = header.str();

        for (auto zone : zones) {
            joiner = zone->get_info(sys_time).abbrev;
        }
        joiner = "\n";
        for (auto zone : zones) {
            (void)zone;
            joiner = " :---: ";
        }
        joiner = "\n";
        for (auto zone : zones) {
            joiner = radio_formatter(date::make_zoned(zone, sys_time));
        }

        std::stringstream ts;
        ts << "\n\nThe discord timestamp for " << event.name << " is: <t:"
           << sys_time.time_since_epoch().count()
           << ":F>\n\n";
        joiner = ts.str();
    }


    const auto sys_time { date::make_zoned("Asia/Tokyo", events[0].date).get_sys_time() };

    joiner = "DJ";
    for (auto zone : zones) {
        joiner = zone->get_info(sys_time).abbrev;
    }
    joiner = "\n";

    joiner = "---";
    for (auto zone : zones) {
        (void)zone;
        joiner = " :---: ";
    }
    joiner = "\n";

//    auto f = [](auto x){return date::format("%H:%M", x);};

/*    for (auto offset : offsets) {
        joiner = offset.DJ;

        for (auto zone : zones) {
            joiner = radio_formatter(date::make_zoned(zone, sys_time + offset.offset));
        }
        joiner = "\n";
    }
*/

    return EXIT_SUCCESS;
}

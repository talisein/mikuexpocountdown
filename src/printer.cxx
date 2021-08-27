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

    constexpr auto expo_local_time = date::local_days{August/29/2021} + 16h;
    const auto expo_sys_time = date::make_zoned("Asia/Tokyo", expo_local_time).get_sys_time();

    struct offset {
        std::string_view DJ;
        std::chrono::minutes offset;
    };

    constexpr std::array<offset, 11> offsets = {{
        {"Balynsus", 0min},
        {"Osanzi", 30min},
        {"Ocelot", 60min},
        {"Camellia", 90min},
        {"Hommarju", 125min},
        {"Fellsius", 155min},
        {"Reno", 185min},
        {"coa white", 215min},
        {"KOTONOHOUSE", 250min},
        {"gaburyu & nyankobrq", 280min},
        {"kz(livetune)", 320min}
    }};

    const std::array<const date::time_zone*, 5> zones = {
            date::locate_zone("America/Los_Angeles"),
            date::locate_zone("America/New_York"),
            date::locate_zone("Europe/London"),
            date::locate_zone("Europe/Paris"),
            date::locate_zone("Asia/Tokyo")
    };
}

int main() {
    auto joiner = std::experimental::make_ostream_joiner(std::cout, "|");

    joiner = "|DJ";
    for (auto zone : zones) {
        joiner = zone->get_info(expo_sys_time).abbrev;
    }
    joiner = "\n";

    for (auto offset : offsets) {
        joiner = offset.DJ;

        auto f = [](auto x){return date::format("%H:%M", x);};
        for (auto zone : zones) {
            joiner = f(date::make_zoned(zone, expo_sys_time + offset.offset));
        }
        joiner = "\n";
    }

    return EXIT_SUCCESS;
}

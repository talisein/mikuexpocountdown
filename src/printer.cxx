#include <chrono>
#include <iostream>
#include <iomanip>
#include <array>
#include <ranges>
#include <experimental/iterator>
#include <string_view>

using namespace std::chrono;

namespace {
    using namespace std::chrono_literals;
    static time_zone const * const JST { locate_zone("Asia/Tokyo") };

    struct event {
        std::string_view name;
        std::string_view short_name;
        std::chrono::local_seconds date;
        std::chrono::minutes duration;
        bool main_event;
    };

    constinit auto events = std::to_array<event>({
            {"Magical Mirai Osaka", "Sunday Night",   local_days{October/14/2024} + 16h + 30min, 2h, true},

        });

    struct offset {
        std::string_view DJ;
        std::chrono::minutes offset;
    };

    constexpr std::array<offset, 11> offsets = {{
        {"OPEN", 0min},
        {"oddrella", 60min},
        {"nyankobrq", 1h + 40min},
        {"Alpaca", 2h + 20min},
        {"gaburyu", 3h},
        {"Yoshihisa Hirata", 3h + 40min},
        {"NUU$HI", 4h + 20min},
        {"Utsu-P", 5h},
        {"hirihiri", 5h + 40min},
        {"colate", 6h + 20min},
        {"CLOSE", 7h},
    }};

    const std::array<const std::chrono::time_zone*, 5> zones = {
//            locate_zone("Asia/Riyadh"),
//            locate_zone("Asia/Kolkata"),
//            locate_zone("Asia/Taipei"),
//            locate_zone("Australia/Sydney"),
//            locate_zone("Australia/Perth"),
            locate_zone("America/Los_Angeles"),
//            locate_zone("America/Denver"),
//            locate_zone("America/Chicago"),
            locate_zone("America/New_York"),
            locate_zone("Europe/London"),
            locate_zone("Europe/Paris"),
            locate_zone("Asia/Tokyo")
    };

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

    std::string
    remove_underscores(const std::string_view sv) {
        std::stringstream out;

        std::replace_copy(sv.begin(), sv.end(),
                          std::ostreambuf_iterator<char>(out),
                          '_', ' ');

        return out.str();
    }

    constexpr std::string_view
    timezone_city(const std::string_view tz)
    {
        return tz.substr(tz.find_first_of('/') + 1);
    }

    std::string
    gcal_link(const event& event)
    {
        std::stringstream ss;
        using namespace std::string_view_literals;
        auto gcal_formatter = [](const auto& x){return std::format("{0:%Y}{0:%m}{0:%d}T{0:%H}{0:%M}00Z", x);};
        const auto jp_time { zoned_time(JST, event.date) };
        const sys_seconds sys_time { jp_time.get_sys_time() };
        ss << "https://calendar.google.com/calendar/u/0/r/eventedit?text="
               << url_encode(event.name)
               << "&dates="
               << gcal_formatter(sys_time)
               << '/'
               << gcal_formatter(sys_time + event.duration)
               << "&ctz=Asia%2FTokyo";
        return ss.str();
    }

    std::string
    gcal_linked_name(const event& event)
    {
        std::stringstream ss;
        if (event.main_event) ss << "**";
        ss << '[' << event.short_name << "](" << gcal_link(event) << ')';
        if (event.main_event) ss << "**";
        return ss.str();
    }

    [[maybe_unused]] std::string
    bold_main_event(const event& event)
    {
        if (!event.main_event)
            return std::string{event.short_name};

        std::stringstream ss;
        ss << "**" << event.short_name << "**";
        return ss.str();
    }

    std::string
    discord_timestamp(const sys_seconds sys_time)
    {
        std::stringstream ss;
        ss << "<t:"
           << sys_time.time_since_epoch().count()
           << ":F>";
        return ss.str();
    }

    std::string
    discord_timestamp_relative(const sys_seconds sys_time)
    {
        std::stringstream ss;
        ss << "<t:"
           << sys_time.time_since_epoch().count()
           << ":R>";
        return ss.str();
    }

    [[maybe_unused]] std::string
    center(const std::string_view s, std::string::difference_type width)
    {
        auto remaining = width - s.size();
        if (remaining <= 0) return std::string(s);
        std::stringstream ss;
        auto pad1 = remaining / 2;
        for (auto i = pad1; i; --i) {
            ss << " ";
        }
        ss << s;
        auto pad2 = remaining - pad1;
        for (auto i = pad2; i; --i) {
            ss << " ";
        }
        return ss.str();
    }
}

int main() {
    auto joiner = std::experimental::make_ostream_joiner(std::cout, "|");
    auto radio_formatter = [](auto x){return std::format("{0:%A} {0:%I}:{0:%M}&nbsp;{0:%p}", x);};
    auto bold_radio_formatter = [](auto x){return std::format("_{0:%A}_ {0:%I}:{0:%M}&nbsp;{0:%p}", x);};
    auto highlight_radio_formatter = [](auto x){return std::format("**{0:%A} {0:%I}:{0:%M}&nbsp;{0:%p}**", x);};
    auto highlight_bold_radio_formatter = [](auto x){return std::format("**_{0:%A}_ {0:%I}:{0:%M}&nbsp;{0:%p}**", x);};
//    auto radio_formatter = [](auto x){return date::format("%A %b&nbsp;%d, %I:%M&nbsp;%p", x);};
//    auto bold_radio_formatter = [](auto x){return date::format("**%A** %b&nbsp;%d, %I:%M&nbsp;%p", x);};
//    auto radio_formatter = [](auto x){return date::format("%I:%M&nbsp;%p", x);};

        joiner = "|Event";
        for (auto zone : zones) {
            joiner = zone->get_info(zoned_time(JST, events[0].date).get_sys_time()).abbrev;
        }
        joiner = "\n";

        joiner = "Google Calendar Link";
        for (auto zone : zones) {
            joiner = remove_underscores(timezone_city(zone->name()));
        }
        joiner = "\n";

        for (size_t i = 0; i < (zones.size()+1); ++i) {
            joiner = " :---: ";
        }
        joiner = "\n";

    for (const auto & event : events) {
         const auto jp_time { zoned_time(JST, event.date) };
         const sys_seconds sys_time { jp_time.get_sys_time() };

//         joiner = bold_main_event(event);//gcal_linked_name(event);
         joiner = gcal_linked_name(event);
        for (auto zone : zones) {
            auto zone_time = zoned_time(zone, sys_time);
            if (event.main_event) {
                if (std::chrono::weekday(floor<days>(zone_time.get_local_time())) != std::chrono::weekday(floor<days>(jp_time.get_local_time()))) {
                    joiner = highlight_bold_radio_formatter(zoned_time(zone, sys_time));
                } else {
                    joiner = highlight_radio_formatter(zoned_time(zone, sys_time));
                }
            } else {
                if (std::chrono::weekday(floor<days>(zone_time.get_local_time())) != std::chrono::weekday(floor<days>(jp_time.get_local_time()))) {
                    joiner = bold_radio_formatter(zoned_time(zone, sys_time));
                } else {
                    joiner = radio_formatter(zoned_time(zone, sys_time));
                }
            }
        }
        joiner = "\n";
    }

    std::cout << "\n\n";

    for (const auto & event : events) {
        const auto jp_time { zoned_time(JST, event.date) };
        const std::chrono::sys_seconds sys_time { jp_time.get_sys_time() };
        joiner = event.short_name;
        joiner = discord_timestamp(sys_time);
        joiner = discord_timestamp_relative(sys_time);
        joiner = "\n";
    }
    std::cout << "\n\n";

    joiner = "DJ";
    for (auto zone : zones) {
        const auto sys_time { zoned_time(JST, events[0].date).get_sys_time() };
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

    for (auto offset : offsets) {
        joiner = offset.DJ;
        const auto jp_time { zoned_time(JST, events[0].date + offset.offset) };
        const auto sys_time { jp_time.get_sys_time() };
        for (auto zone : zones) {
            const auto zone_time { zoned_time(zone, sys_time) };
            if (std::chrono::weekday(floor<days>(zone_time.get_local_time())) != std::chrono::weekday(floor<days>(jp_time.get_local_time()))) {
                joiner = bold_radio_formatter(zone_time);
            } else {
                joiner = radio_formatter(zone_time);
            }
        }
        joiner = "\n";
    }


    return EXIT_SUCCESS;
}

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

    constexpr int remaining_songs = 33;
    constinit std::array<const event, 2> events {{
            {"Miku Expo Rewind", local_days{June/5/2022} + 14h, 3h},
            {"33 tweet", local_days{May/2/2022} + 12h + 38min, 6h},
        }};

    const date::time_zone* jst = date::locate_zone("Asia/Tokyo");
    const date::time_zone* pst = date::locate_zone("America/Los_Angeles");

    constinit std::array<std::string_view, 33> songs = {
        "Fragments of a Star",
        "I'll Miku-Miku You♪ (For Reals)",
        "Kimagure Mercy",
        "Meltdown",
        "Miku",
        "ODDS&ENDS",
        "Pane dhiria",
        "Satisfaction",
        "Snowman",
        "Ten Thousand Stars",
        "Change me",
        "Glass Wall",
        "Last Night, Good Night",
        "The Disappearance of Hatsune Miku -DEAD END-",
        "Weekender Girl",
        "39 (Thank You)",
        "Blue Star",
        "Melt",
        "Romeo and Cinderella",
        "Sharing The World",
        "Thousand Cherry Blossoms",
        "Tokyo Teddy Bear",
        "Two-Sided Lovers",
        "Unhappy Refrain",
        "World's End Dancehall",
        "Luka Luka★Night Fever",
        "Just Be Friends",
        "Secret Police",
        "The Intense Voice of Hatsune Miku",
        "Remote Controller",
        "Butterfly on Your Right Shoulder",
        "Tell Your World",
        "World is Mine",
    };
}

int main()
{

    const auto showtime { date::make_zoned(jst, events[0].date) };
    const auto tweettime { date::make_zoned(pst, events[1].date) };

    auto span = showtime.get_sys_time() - tweettime.get_sys_time() - 24h;


    const auto intertweet_duration = span / remaining_songs;

    size_t remaining = 33;
    auto song = std::begin(songs);
    for (auto tt = tweettime.get_sys_time() + intertweet_duration;
         tt < showtime.get_sys_time() && song < std::end(songs);
         tt += intertweet_duration, --remaining)
    {
//        auto dur =             std::chrono_cast<std::chrono::days>(showtime.get_sys_time() - tt)
        auto pure_dur = showtime.get_sys_time() - tt;
        auto dur = std::chrono::floor<std::chrono::days>(showtime.get_sys_time() - tt);
        auto hours = std::chrono::floor<std::chrono::hours>(pure_dur - dur);
        auto min = std::chrono::round<std::chrono::minutes>(pure_dur - dur - hours);

        std::cout << date::make_zoned(pst, tt) << ": " << remaining << " songs and " <<
            std::chrono::round<std::chrono::days>(pure_dur) << " days (" <<
            dur << " " << hours << " " << min <<
            ") remaining. This song is " << *song++ << std::endl;
    }

    std::cout << std::chrono::round<std::chrono::minutes>(intertweet_duration - 23h) << " min between tweets.\n";

    return 0;
}

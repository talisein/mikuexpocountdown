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
            {"Miku Expo Rewind", local_days{June/5/2022} + 10h + 30min, 3h},
            {"20 tweet", local_days{May/7/2022} + 10h + 49min, 6h},
        }};

    const date::time_zone* jst = date::locate_zone("Asia/Tokyo");
    const date::time_zone* pst = date::locate_zone("America/Los_Angeles");

    constinit std::array<std::string_view, 28> songs = {
        "ODDS&ENDS",
        "Pane dhiria",
        "Satisfaction",
        "Snowman",
        "Ten Thousand Stars",
        "Change me", /* 5 times */
        "Glass Wall",
        "Last Night, Good Night",
        "The Disappearance of Hatsune Miku -DEAD END-",
        "Weekender Girl",
        "39 (Thank You)", /* 6 times */
        "Blue Star",
        "Melt",
        "Romeo and Cinderella",
        "Sharing The World",
        "Thousand Cherry Blossoms",
        "Tokyo Teddy Bear",
        "Two-Sided Lovers",
        "Unhappy Refrain",
        "World's End Dancehall",
        "Luka Luka★Night Fever", /* 7 times */
        "Just Be Friends", /* 8 times */
        "Secret Police",
        "The Intense Voice of Hatsune Miku",
        "Remote Controller", /* 9 times */
        "Butterfly on Your Right Shoulder", /* 10 times */
        "Tell Your World", /* 12 times */
        "World is Mine", /* 12 times */
    };
    constexpr int remaining_songs = songs.size();

}

int main()
{

    const auto showtime { date::make_zoned(jst, events[0].date) };
    const auto tweettime { date::make_zoned(pst, events[1].date) };

    auto span = showtime.get_sys_time() - tweettime.get_sys_time() - 24h;


    const auto intertweet_duration = span / remaining_songs;

    size_t remaining = songs.size();
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

        std::cout << date::make_zoned(pst, tt) << ": " << remaining << " songs and ";
        date::operator<<(std::cout, std::chrono::floor<std::chrono::days>(pure_dur));
        std::cout << " days (";
        date::operator<<(std::cout, dur);
        std::cout << " ";
        date::operator<<(std::cout, hours);
        std::cout << " ";
        date::operator<<(std::cout, min);
        std::cout << ") remaining. This song is " << *song++ << std::endl;
    }

    date::operator<<(std::cout, std::chrono::floor<std::chrono::minutes>(intertweet_duration - 23h));
    std::cout << " min between tweets.\n";

    return 0;
}

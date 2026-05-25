#include <chrono>
#include <iostream>
#include <iomanip>
#include <array>
#include <ranges>
#include <experimental/iterator>

namespace {
    using namespace std::chrono_literals;

    struct event {
        std::string_view name;
        std::chrono::local_seconds date;
        std::chrono::minutes duration;
    };

    constinit std::array<const event, 2> events {{
            {"Miku Expo Rewind", std::chrono::local_days{std::chrono::June/5/2022} + 10h + 30min, 3h},
            {"25 tweet", std::chrono::local_days{std::chrono::May/10/2022} + 18h + 30min, 6h},
        }};

    const std::chrono::time_zone* jst = std::chrono::locate_zone("Asia/Tokyo");
    const std::chrono::time_zone* pst = std::chrono::locate_zone("America/Los_Angeles");

    constinit std::array<std::string_view, 24> songs = {
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

    const auto showtime { std::chrono::zoned_time(jst, events[0].date) };
    const auto tweettime { std::chrono::zoned_time(pst, events[1].date) };

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

        std::cout << std::chrono::zoned_time(pst, tt) << ": " << remaining << " songs and "
                  << std::chrono::floor<std::chrono::days>(pure_dur)
                  << " days ("
                  << dur
                  << " "
                  << hours
                  << " "
                  << min
                  << ") remaining. This song is "
                  << *song++
                  << std::endl;
    }

    std::cout << std::chrono::floor<std::chrono::minutes>(intertweet_duration - 23h)
              << " min between tweets.\n";

    return 0;
}

#pragma once
#include <chrono>
#include <vector>
#include <ranges>
#include <glibmm.h>

#if __cpp_lib_chrono < 201907
#include "date/date.h"
#include "date/tz.h"
#endif

namespace Miku
{
    struct raw_event;

    class Event : public Glib::Object
    {
    private:
        Event(const raw_event& event);

    public:
        static Glib::RefPtr<Event> create(const raw_event& event);

        virtual ~Event() = default;

        inline bool is_live() const { using namespace std::chrono_literals; return 0s > secs_to_live(); }

        inline bool is_expired() const { using namespace std::chrono_literals; return 0s > secs_to_expire(); }

        inline bool is_unexpired() const { return !is_expired(); }

        inline std::chrono::seconds secs_to_live() const {
            return floor<std::chrono::seconds>(start_time.get_sys_time()
                                               - std::chrono::system_clock::now());
        }

        inline std::chrono::seconds secs_to_expire() const {
            return secs_to_live() + m_duration;
        }

        date::zoned_seconds get_expire_time() const;

        bool match_search(const std::vector<Glib::ustring> &terms) const;

        Glib::ustring name;
        Glib::ustring m_style_class;
        Glib::ustring search_keys;
        date::zoned_seconds start_time;
        date::zoned_seconds end_time;
        std::chrono::minutes m_duration;

    private:
        date::local_seconds m_local_time;
    };

    const std::vector<Glib::RefPtr<const Miku::Event>>& get_events();
}

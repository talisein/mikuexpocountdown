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
        template <typename... Args>
        static Glib::RefPtr<Event> create(Args&&... args) {
            struct enabler : public Miku::Event {
                enabler(Args&&... argz) : Miku::Event(std::forward<Args...>(argz...)) {};
            };
            return std::make_shared<enabler>(std::forward<Args...>(args...));
        }

        virtual ~Event() override = default;

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

        inline bool match_search(const std::vector<Glib::ustring> &terms) const {
            using namespace std::ranges;
            const auto match_keys = [this](const auto& term) -> bool
            {
                return Glib::ustring::npos != search_keys.find(term);
            };
            return all_of(views::transform(terms, &Glib::ustring::casefold), match_keys);
        }

        Glib::ustring name;
        Glib::ustring m_style_class;
        Glib::ustring search_keys;
        date::zoned_seconds start_time;
        date::zoned_seconds end_time;
        std::chrono::minutes m_duration;

    private:
        date::local_seconds m_local_time;
    };

    using events_view_t = std::ranges::ref_view<const std::vector<Glib::RefPtr<const Event>>>;
    events_view_t get_events();
}

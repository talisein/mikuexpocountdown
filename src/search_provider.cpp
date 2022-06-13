#include <iostream>
#include <ranges>
#include "search_provider.h"
#include "events.h"

namespace {
    std::vector<Glib::ustring> _search(const std::vector<Glib::ustring>& terms) {
      using namespace std::ranges;
      constexpr auto get_name = [](const auto &e) { return e->name; };
      auto wow = Miku::get_events()
          | views::filter(&Miku::Event::is_unexpired)
          | views::filter([&terms](const auto& e) { return e->match_search(terms); })
          | views::transform(get_name);

        return {wow.begin(), wow.end()};
    }
}

void
SearchProvider::GetInitialResultSet(const std::vector<Glib::ustring> & terms,
                                    MethodInvocation &invocation)
{
    invocation.ret(_search(terms));
}

void
SearchProvider::GetSubsearchResultSet(const std::vector<Glib::ustring> & previous_results,
                                      const std::vector<Glib::ustring> & terms,
                                      MethodInvocation &invocation)
{
    invocation.ret(_search(terms));
}

void
SearchProvider::GetResultMetas(const std::vector<Glib::ustring> & identifiers,
                               MethodInvocation &invocation)
{
    std::vector<std::map<Glib::ustring,Glib::VariantBase>> v;
    for (auto &id : identifiers) {
        auto it = std::ranges::find_if(Miku::get_events(), [&id](const Glib::RefPtr<const Miku::Event> &event) {
            return event->name.compare(id) == 0; });
        if (std::end(Miku::get_events()) != it) {
            std::map<Glib::ustring, Glib::VariantBase> m;
            m.insert({"id", Glib::Variant<Glib::ustring>::create(id)});
            m.insert({"name", Glib::Variant<Glib::ustring>::create((*it)->name)});
            m.insert({"gicon", Glib::Variant<Glib::ustring>::create("dance._39music.MikuExpoCountdown")});
            auto diff_secs = (*it)->secs_to_live();
            auto diff_days = floor<std::chrono::days>(diff_secs);

            std::stringstream ss;
            if (diff_days.count() > 0) {
                ss << "in about " << diff_days.count() << " day";
                if (diff_days.count() > 1)
                    ss << 's';
            } else {
                auto diff_hours = floor<std::chrono::hours>(diff_secs);
                if (diff_hours.count() > 0) {
                    ss << "in about " << diff_hours.count() << " hour";
                    if (diff_hours.count() > 1)
                        ss << 's';
                } else {
                    auto diff_minutes = floor<std::chrono::minutes>(diff_secs);
                    if (diff_minutes.count() < 1) {
                        if (diff_secs.count() <= 0) {
                            if (!(*it)->is_expired()) {
                                ss << "Happening now";
                            } else {
                                ss << "Event finished";
                            }
                        } else {
                            ss << "in less than a minute!";
                        }
                    } else if (diff_minutes.count() == 1) {
                        ss << "in a minute!";
                    } else {
                        ss << "in about " << diff_minutes.count() << " minutes";
                    }
                }
            }
            Glib::ustring display_date;
            if (diff_secs.count() <= 0) { // Event started
                auto end_time = (*it)->end_time;
                if (!(*it)->is_expired()) { // Event ongoing
                    display_date = Glib::ustring::compose("until %1", date::format("%c %Z", date::make_zoned(date::current_zone(), end_time)));
                } else { // Event finished
                    display_date = Glib::ustring::compose("ended %1", date::format("%c %Z", date::make_zoned(date::current_zone(), end_time)));
                }
            } else { // Event starts in the future
                display_date = date::format("%c %Z", date::make_zoned(date::current_zone(), (*it)->start_time));
            }
            auto description = Glib::ustring::compose("%1 \u2014 %2",
                                                      ss.str(),
                                                      display_date);
            m.insert({"description", Glib::Variant<Glib::ustring>::create(description)});
            v.push_back(m);
        }
    }
    invocation.ret(v);
}

void
SearchProvider::ActivateResult(const Glib::ustring & identifier,
                               const std::vector<Glib::ustring> & terms,
                               guint32 timestamp,
                               MethodInvocation &invocation)
{
    invocation.ret();
}

void
SearchProvider::LaunchSearch(const std::vector<Glib::ustring> & terms,
                             guint32 timestamp,
                             MethodInvocation &invocation)
{
    invocation.ret();
}

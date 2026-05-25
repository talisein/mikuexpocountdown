#include <chrono>
#include <format>
#include <peel/GLib/functions.h>
#include <peel/Pango/AttrList.h>
#include <peel/Pango/functions.h>
#include <peel/Gtk/Align.h>
#include <peel/Gtk/Justification.h>
#include <peel/Gtk/Orientable.h>
#include <peel/Gtk/Orientation.h>
#include "grid.h"
#include "util.hpp"

PEEL_CLASS_IMPL(CountdownGrid, "MikuCountdownGrid", peel::Gtk::Box)

peel::Signal<CountdownGrid, void()> CountdownGrid::s_signal_expired;

void CountdownGrid::Class::init() {
    override_vfunc_dispose<CountdownGrid>();
    s_signal_expired = peel::Signal<CountdownGrid, void()>::create("expired");
}

void CountdownGrid::init(Class *) {
    new (&m) Members;
}

void CountdownGrid::vfunc_dispose() {
    if (m.update_timeout_id) {
        peel::GLib::Source::remove(m.update_timeout_id);
        m.update_timeout_id = 0;
    }
    parent_vfunc_dispose<CountdownGrid>();
}

void CountdownGrid::setup(const peel::RefPtr<Miku::Event> &event) {
    m.event = event;

    auto days_fp  = peel::Gtk::Label::create("");    m.days       = days_fp;
    auto hours_fp = peel::Gtk::Label::create("");    m.hours      = hours_fp;
    auto date_fp  = peel::Gtk::Label::create("");    m.date       = date_fp;
    auto box_fp   = peel::Gtk::CenterBox::create();  m.bottom_box = box_fp;

    add_css_class(event->style_class());
    cast<peel::Gtk::Orientable>()->set_orientation(peel::Gtk::Orientation::VERTICAL);
    set_name("page");

    m.days->set_name("days");
    m.days->add_css_class(event->style_class());
    m.days->set_max_width_chars(8);
    m.days->set_wrap(true);
    m.days->set_justify(peel::Gtk::Justification::CENTER);
    m.days->set_vexpand(true);
    m.days->set_valign(peel::Gtk::Align::CENTER);

    auto attrs = peel::Pango::AttrList::create();
    attrs->insert(peel::Pango::attr_line_height_new(0.6));
    m.days->set_attributes(attrs);

    m.hours->set_name("hours");
    m.date->set_name("date");
    m.date->set_valign(peel::Gtk::Align::END);

    m.bottom_box->set_valign(peel::Gtk::Align::FILL);
    m.bottom_box->set_vexpand(true);
    m.bottom_box->cast<peel::Gtk::Orientable>()->set_orientation(peel::Gtk::Orientation::VERTICAL);
    m.bottom_box->set_center_widget(std::move(hours_fp));
    m.bottom_box->set_end_widget(std::move(date_fp));

    append(std::move(days_fp));
    append(std::move(box_fp));

    auto event_localtime = m.event->start_time();
    auto user_localtime  = std::chrono::zoned_time(std::chrono::current_zone(), event_localtime);
    static constexpr char date_format[] { "{0:%A} {0:%d} {0:%B} {0:%Y} {0:%X} {0:%Z}" };
    m.date->set_text(format_to_peel("{}\n{}",
        std::format(date_format, user_localtime),
        std::format(date_format, event_localtime)));

    update();
    m.update_timeout_id = peel::GLib::timeout_add_seconds(1, [this]() -> bool {
        return update();
    });
}

bool CountdownGrid::update() {
    using namespace std::chrono_literals;
    auto diff_secs = m.event->secs_to_live();
    auto diff_days = std::chrono::floor<std::chrono::days>(diff_secs);

    if (diff_days > 0s) {
        m.hours->set_visible(true);
        if (diff_days.count() > 1)
            m.days->set_label(format_to_peel("{} days", diff_days.count()));
        else
            m.days->set_label(format_to_peel("{} day", diff_days.count()));
        m.hours->set_label(format_to_peel("{:%T}", diff_secs - diff_days));
    } else {
        if (diff_secs > 0s) {
            auto concert_weekday = std::chrono::weekday(std::chrono::floor<std::chrono::days>(std::chrono::zoned_time(std::chrono::current_zone(), m.event->start_time()).get_local_time()));
            auto today_weekday = std::chrono::weekday(std::chrono::floor<std::chrono::days>(std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()).get_local_time()));
            if (concert_weekday == today_weekday)
                m.days->set_label("Today");
            else
                m.days->set_label("Soon");
            m.hours->set_label(format_to_peel("{:%T}", diff_secs));
        } else {
            m.hours->set_visible(false);
            m.days->set_label("Miku time now!");
            auto secs_to_end = m.event->secs_to_expire();
            if (secs_to_end > 0s) {
                peel::GLib::timeout_add_seconds_once(secs_to_end.count(), [this]() {
                    s_signal_expired.emit(this);
                });
            } else {
                s_signal_expired.emit(this);
            }
            return false;
        }
    }

    return true;
}

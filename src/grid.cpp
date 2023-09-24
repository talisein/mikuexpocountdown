#include <iostream>
#include "grid.h"
#include "adwaita.h"

CountdownGrid::CountdownGrid(const Glib::RefPtr<const Miku::Event> &event) :
    Glib::ObjectBase("CountdownGrid"),
    Gtk::CenterBox(),
    m_event(event),
    m_days(Gtk::make_managed<Gtk::Label>()),
    m_hours(Gtk::make_managed<Gtk::Label>()),
    m_date(Gtk::make_managed<Gtk::Label>()),
    m_bottom_box(Gtk::make_managed<Gtk::CenterBox>())
{
    set_orientation(Gtk::Orientation::VERTICAL);
    set_name("page");
    m_days->set_name("days");
    m_hours->set_name("hours");
    m_date->set_name("date");
    set_center_widget(*m_days);
    set_end_widget(*m_bottom_box);
    m_bottom_box->set_valign(Gtk::Align::FILL);
    m_bottom_box->set_vexpand(true);
    m_bottom_box->set_orientation(Gtk::Orientation::VERTICAL);
    m_bottom_box->set_center_widget(*m_hours);
    m_bottom_box->set_end_widget(*m_date);
    m_date->set_valign(Gtk::Align::END);

    m_days->set_max_width_chars(8); // '999 days'
    m_days->set_wrap(true);
    m_days->set_justify(Gtk::Justification::CENTER);
    auto attrs = Pango::AttrList();
    auto line_height = Pango::Attribute::create_attr_line_height(0.6);
    attrs.insert(line_height);
    m_days->set_attributes(attrs);

    add_css_class(event->m_style_class);

    auto event_localtime = m_event->start_time;
    auto user_localtime  = date::make_zoned(date::current_zone(), event_localtime);
    constexpr char date_format[] { "%A %d %B %Y %X %Z" };

    m_date->set_text(Glib::ustring::compose("%1\n%2",
                                            date::format(date_format, user_localtime),
                                            date::format(date_format, event_localtime)));
    update();
    update_timeout_connection = Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &CountdownGrid::update), 1);
}

bool
CountdownGrid::update()
{
    using namespace std::chrono_literals;
    auto diff_secs = m_event->secs_to_live();
    auto diff_days = floor<std::chrono::days>(diff_secs);

    if (diff_days > 0s) {
        m_hours->show();
        if (diff_days.count() > 1) {
            m_days->set_label(Glib::ustring::compose("%1 days", diff_days.count()));
        } else {
            m_days->set_label(Glib::ustring::compose("%1 day", diff_days.count()));
        }
        m_hours->set_label(date::format("%T", diff_secs - diff_days));
    } else {
        if (diff_secs > 0s) {
            auto concert_weekday = date::weekday(std::chrono::floor<std::chrono::days>(date::make_zoned(date::current_zone(), m_event->start_time).get_local_time()));
            auto today_weekday = date::weekday(std::chrono::floor<std::chrono::days>(date::make_zoned(date::current_zone(), std::chrono::system_clock::now()).get_local_time()));
            if (concert_weekday == today_weekday) {
                m_days->set_label("Today");
            } else {
                m_days->set_label("Soon");
            }
            m_hours->set_label(date::format("%T", diff_secs));
        } else {
            m_hours->hide();
            m_days->set_label("Miku time now!");
            auto secs_to_end = m_event->secs_to_expire();
            if (secs_to_end > 0s) {
                Glib::signal_timeout().connect_seconds_once(sigc::mem_fun(m_signal_expired, &decltype(m_signal_expired)::emit),
                                                            secs_to_end.count());
            } else {
                m_signal_expired.emit();
            }
            return false;
        }
    }

    return true;
}

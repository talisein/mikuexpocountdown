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

    auto context = get_style_context();
    context->add_class(event->m_style_class);
    context = get_style_context();
    context->add_class(event->m_style_class);

    auto jsttime   = m_event->start_time;
    auto localtime = date::make_zoned(date::current_zone(), jsttime);
    m_date->set_text(Glib::ustring::compose("%1\n%2",
                                            date::format("%c %Z", localtime),
                                            date::format("%c %Z", jsttime)));
    update();
    Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &CountdownGrid::update), 1);
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
            m_days->set_label("Soon");
            m_hours->set_label(date::format("%T", diff_secs));
        } else {
            m_hours->hide();
            m_days->set_label("Miku time now!");
            auto secs_to_end = m_event->secs_to_expire();
            if (secs_to_end > 0s) {
                Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &CountdownGrid::remove_me), secs_to_end.count());
            } else {
                remove_me();
            }
            return false;
        }
    }

    return true;
}

bool
CountdownGrid::remove_me()
{
    auto stack_widget = dynamic_cast<Gtk::Widget*>(this->get_parent());
    AdwViewStack *stack = ADW_VIEW_STACK(stack_widget);
    if (stack) {
//        auto visible_child = adw_view_stack_get_visible_child(stack);
        adw_view_stack_remove (stack, static_cast<Gtk::Widget*>(this)->gobj());
//        if (static_cast<Gtk::Widget*>(this)->gobj() == visible_child) {
//            adw_view_stack_set_visible_child(stack, adw_view_stack_get_first_child(stack));
//            stack->set_visible_child(*(stack->get_first_child()));
//        }
    }
    return false;
}

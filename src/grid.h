#pragma once
#include <gtkmm.h>
#include "events.h"

class CountdownGrid : public Gtk::CenterBox
{
public:
    CountdownGrid(const Glib::RefPtr<const Miku::Event> &event);

    bool update();
    bool remove_me() {
        auto stack = dynamic_cast<Gtk::Stack*>(this->get_parent());
        if (stack) {
            auto visible_child = stack->get_visible_child();
            stack->remove(*this);
            if (static_cast<Gtk::Widget*>(this) == visible_child) {
                stack->set_visible_child(*(stack->get_first_child()));
            }
        }
        return false;
    }
    Glib::RefPtr<const Miku::Event> m_event;

private:
    Gtk::Label *m_days;
    Gtk::Label *m_hours;
    Gtk::Label *m_date;
    Gtk::CenterBox *m_bottom_box;
};

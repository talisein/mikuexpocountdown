#pragma once
#include <gtkmm.h>
#include "events.h"

class CountdownGrid final : public Gtk::CenterBox
{
public:
    CountdownGrid(const Glib::RefPtr<const Miku::Event> &event);

    bool update();
    bool remove_me();

    Glib::RefPtr<const Miku::Event> m_event;

    virtual ~CountdownGrid() override final = default;
private:
    Gtk::Label *m_days;
    Gtk::Label *m_hours;
    Gtk::Label *m_date;
    Gtk::CenterBox *m_bottom_box;
};

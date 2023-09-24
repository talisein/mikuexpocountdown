#pragma once
#include <gtkmm.h>
#include "events.h"

class CountdownGrid final : public Gtk::CenterBox
{
public:
    CountdownGrid(const Glib::RefPtr<const Miku::Event> &event);

    bool update();

    Glib::RefPtr<const Miku::Event> m_event;

    sigc::signal<void()> signal_expired() { return m_signal_expired; }

    virtual ~CountdownGrid() override final = default;
private:
    Gtk::Label *m_days;
    Gtk::Label *m_hours;
    Gtk::Label *m_date;
    Gtk::CenterBox *m_bottom_box;
    sigc::signal<void()> m_signal_expired;
    sigc::connection update_timeout_connection;

};

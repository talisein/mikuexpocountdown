#pragma once
#include <peel/class.h>
#include <peel/signal.h>
#include <peel/Gtk/Box.h>
#include <peel/Gtk/Label.h>
#include <peel/Gtk/CenterBox.h>
#include <peel/GLib/Quark.h>
#include "events.h"

class CountdownGrid final : public peel::Gtk::Box
{
    PEEL_SIMPLE_CLASS(CountdownGrid, peel::Gtk::Box);

    struct Members {
        peel::RefPtr<Miku::Event> m_event;
        peel::Gtk::Label    *m_days      = nullptr;
        peel::Gtk::Label    *m_hours     = nullptr;
        peel::Gtk::Label    *m_date      = nullptr;
        peel::Gtk::CenterBox *m_bottom_box = nullptr;
        unsigned             m_update_timeout_id = 0;
    } *m = nullptr;

    static peel::Signal<CountdownGrid, void()> s_signal_expired;

    void init(Class *);
    void vfunc_dispose();
    ~CountdownGrid() { delete m; }

    bool update();

public:
    void setup(const peel::RefPtr<Miku::Event> &event);

    static CountdownGrid *create(const peel::RefPtr<Miku::Event> &event) {
        auto g = peel::GObject::Object::create<CountdownGrid>();
        g->setup(event);
        return std::move(g).release_floating_ptr();
    }

    const char *event_name() const { return m->m_event->name(); }

    template<typename H>
    peel::SignalConnection::Token connect_expired(H &&h) {
        return s_signal_expired.connect(this, peel::GLib::Quark{}, std::forward<H>(h));
    }
};

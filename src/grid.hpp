#pragma once
#include <peel/class.h>
#include <peel/signal.h>
#include <peel/widget-template.h>
#include <peel/Gtk/Box.h>
#include <peel/Gtk/Label.h>
#include "events.hpp"

class CountdownGrid final : public peel::Gtk::Box
{
    PEEL_SIMPLE_CLASS(CountdownGrid, peel::Gtk::Box);

    struct Members {
        peel::RefPtr<Miku::Event> event;
        peel::Gtk::Label    *days      = nullptr;
        peel::Gtk::Label    *hours     = nullptr;
        peel::Gtk::Label    *date      = nullptr;
        unsigned             update_timeout_id = 0;
    } m;

    static peel::Signal<CountdownGrid, void()> signal_expired;

    void init(Class *);
    void vfunc_dispose();

    bool update();

    template<typename F>
    static void define_properties(F &f) {
        f.prop(prop_event_name(), (const char*)nullptr).get(&CountdownGrid::event_name);
    }

public:
    PEEL_PROPERTY(const char*, event_name, "event-name")

    void setup(const peel::RefPtr<Miku::Event> &event);

    static CountdownGrid *create(const peel::RefPtr<Miku::Event> &event) {
        auto g = peel::GObject::Object::create<CountdownGrid>();
        g->setup(event);
        return std::move(g).release_floating_ptr();
    }

    peel::String event_name() const { return m.event->get_property(Miku::Event::prop_name()); }

    PEEL_SIGNAL_CONNECT_METHOD(expired, signal_expired);
};

#pragma once

#include <ranges>
#include <gtkmm.h>

#include "grid.h"
#include "events.h"

class CountdownWindow final : public Gtk::ApplicationWindow
{
public:
CountdownWindow(const Glib::RefPtr<Gtk::Application>& app) :
    Glib::ObjectBase("CountdownWindow"),
        Gtk::ApplicationWindow(app),
        m_box(Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL)),
        m_stack(Gtk::make_managed<Gtk::Stack>()),
        m_stack_switcher(Gtk::make_managed<Gtk::StackSwitcher>())
    {
        set_name("mainwin");
        m_box->append(*m_stack_switcher);
        m_box->append(*m_stack);
        m_stack_switcher->set_stack(*m_stack);
        m_stack_switcher->set_halign(Gtk::Align::CENTER);
        auto provider = Gtk::CssProvider::create();
        provider->load_from_resource("/org/talinet/mikuexpocountdown/style.css");
        auto style_context = get_style_context();
        for (auto &display : Gdk::DisplayManager::get()->list_displays()) {
            style_context->add_provider_for_display(display, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }

        set_child(*m_box);
        m_stack->set_expand(true);
        m_stack->property_visible_child().signal_changed().connect(sigc::mem_fun(*this, &CountdownWindow::on_switch));

        using namespace std::ranges;
        for (const auto &e : Miku::get_events() | views::filter(&Miku::Event::is_unexpired)) {
            auto grid = Gtk::make_managed<CountdownGrid>(e);
            m_stack->add(*grid, e->name, e->name);
        }
    }

    virtual ~CountdownWindow() override final = default;

private:
    Gtk::Box *m_box;
    Gtk::Stack *m_stack;
    Gtk::StackSwitcher *m_stack_switcher;

    void on_switch() {
        auto event = static_cast<CountdownGrid*>(m_stack->property_visible_child().get_value())->m_event;
        auto style_class = event->m_style_class;
        auto context = m_box->get_style_context();
        for (auto &e : Miku::get_events()) {
            if (context->has_class(e->m_style_class)) context->remove_class(e->m_style_class);
        }
        context->add_class(style_class);
        set_title(Glib::ustring::compose("%1 Countdown", event->name));
    }
};

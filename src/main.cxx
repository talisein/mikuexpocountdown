#include <version>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <gtkmm.h>

#if __cpp_lib_chrono < 201907
#include "date/date.h"
#include "date/tz.h"
#endif

#include "application.h"
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

CountdownGrid::CountdownGrid(const Glib::RefPtr<const Miku::Event> &event) :
    Glib::ObjectBase("CountdownGrid"),
    Gtk::CenterBox(),
    m_event(event),
    m_days(Gtk::manage(new Gtk::Label())),
    m_hours(Gtk::manage(new Gtk::Label())),
    m_date(Gtk::manage(new Gtk::Label())),
    m_bottom_box(Gtk::manage(new Gtk::CenterBox()))
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
}

bool CountdownGrid::update()
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

class CountdownWindow : public Gtk::ApplicationWindow
{
public:
    CountdownWindow(const Glib::RefPtr<Gtk::Application>& app) :
        Glib::ObjectBase("CountdownWindow"),
        Gtk::ApplicationWindow(app),
        m_box(Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL))),
        m_stack(Gtk::manage(new Gtk::Stack())),
        m_stack_switcher(Gtk::manage(new Gtk::StackSwitcher()))
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

        for (const auto &e : Miku::get_events() | std::ranges::views::filter(&Miku::Event::is_unexpired)) {
            auto grid = Gtk::manage(new CountdownGrid(e));
            m_stack->add(*grid, e->name, e->name);
            Glib::signal_timeout().connect_seconds(sigc::mem_fun(*grid, &CountdownGrid::update), 1);
        }
    }

    virtual ~CountdownWindow() = default;

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

int main (int argc, char *argv[])
{
    auto app = MikuApplication::create("dance._39music.MikuExpoCountdown");

    if constexpr (!GTKMM_CHECK_VERSION(4,8,0)) {
        Gtk::Window *m_run_window = nullptr;
        app->signal_activate().connect([&app, &m_run_window] () {
            if (!m_run_window) {
                auto window = new CountdownWindow(app);
                bool was_floating = g_object_is_floating(window->gobj());
                window->Gtk::Object::set_manage();
                if (!was_floating && g_object_is_floating(window->gobj()))
                    g_object_ref_sink(window->gobj());
                m_run_window = window;
                app->add_window(*window);
                window->set_application(app);
                window->present();
            } else {
                m_run_window->present();
            }
        });

        app->signal_window_removed().connect([&m_run_window] (Gtk::Window* window) {
            if (window == m_run_window)
            {
                if (window && window->gobj())
                    gtk_window_destroy(window->gobj());
                m_run_window = nullptr;
            }
        });

        app->run(argc, argv);
    } else {
        return app->make_window_and_run<CountdownWindow>(argc, argv, app);
    }
}

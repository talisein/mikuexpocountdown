#include <chrono>
#include <iostream>
#include <iomanip>
#include <gtkmm.h>
#include <version>

#if __cpp_lib_chrono < 201907
#include "date/date.h"
#include "date/tz.h"
#endif

namespace {
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono_literals;

    constexpr auto mer_local_time = date::local_days{June/5/2022} + 10h + 30min;
    const auto JST = date::locate_zone("Asia/Tokyo");

    struct raw_event {
        std::string_view name;
        std::string_view style_class;
        date::local_seconds time;
        std::chrono::minutes duration;
    };

    constinit std::array<const raw_event, 5> events {{
            {"Miku Expo", "expo", date::local_days{June/5/2022} + 10h + 30min, 2h + 90min},
            {"Digital Stars", "digistars", local_days{May/28/2022} + 13h, 6h},
            {"Mirai Osaka", "mirai", local_days{August/12/2022} + 12h, 48h + 12h},
            {"Mirai Tokyo", "mirai", local_days{September/2/2022} + 12h, 48h + 12h},
            {"Mirai Sapporo", "mirai", local_days{February/4/2023} + 12h, 48h},
        }};
}



class Event : public Glib::Object
{
private:
    Event(const raw_event& event) :
        Glib::ObjectBase("event"),
        property_name(*this, "name", {std::begin(event.name), std::end(event.name)}),
        m_style_class(std::begin(event.style_class), std::end(event.style_class)),
        property_time(*this, "time", {JST, event.time}),
        m_local_time(event.time)
    {
    }

public:
    static Glib::RefPtr<Event> create(const raw_event& event)
    {
        return Glib::RefPtr<Event>(new Event(event));
    }
    virtual ~Event() = default;

    Glib::Property<Glib::ustring> property_name;
    Glib::ustring m_style_class;
    Glib::Property<date::zoned_seconds> property_time;

private:
    date::local_seconds m_local_time;
};

class CountdownGrid : public Gtk::Box
{
public:
    CountdownGrid(const Glib::RefPtr<Event> &event);

    bool update();

    Glib::RefPtr<Event> m_event;
private:
    Gtk::Grid *m_grid;
    Gtk::Label *m_days;
    Gtk::Label *m_hours;
};

CountdownGrid::CountdownGrid(const Glib::RefPtr<Event> &event) :
    m_event(event),
    m_grid(Gtk::manage(new Gtk::Grid())),
    m_days(Gtk::manage(new Gtk::Label())),
    m_hours(Gtk::manage(new Gtk::Label()))
{
    set_name("page");
    m_days->set_name("days");
    m_hours->set_name("hours");

    m_grid->attach(*m_days, 0, 0);
    m_grid->attach(*m_hours, 0, 1);
    m_grid->set_halign(Gtk::ALIGN_CENTER);
    m_grid->set_valign(Gtk::ALIGN_CENTER);
    m_grid->set_hexpand(true);
    m_grid->set_vexpand(true);
    add(*m_grid);

    auto context = get_style_context();
    context->add_class(event->m_style_class);
    context = m_grid->get_style_context();
    context->add_class(event->m_style_class);
    show_all();
}

bool CountdownGrid::update()
{
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono;

    auto time = m_event->property_time.get_value();
    auto sys_time = time.get_sys_time();
    auto diff_secs = floor<seconds>(sys_time - system_clock::now());
    auto diff_days = floor<std::chrono::days>(diff_secs);

    if (diff_days.count() > 0) {
        std::stringstream ss;
        ss << diff_days.count() << " ";
        if (diff_days.count() > 1) {
            ss << "days";
        } else {
            ss << "day";
        }
        m_days->set_label(ss.str());
    } else {
        if (diff_secs.count() > 0) {
            m_days->set_line_wrap(true);
            m_days->set_line_wrap_mode(Pango::WRAP_WORD);
            m_days->set_label("Soon");
        } else {
            m_days->set_label("Miku Miku Miku Miku Miku Miku Miku Miku Miku Miku");
            m_days->set_line_wrap(true);
            m_days->set_lines(5);
            m_grid->set_valign(Gtk::ALIGN_START);
            m_grid->set_halign(Gtk::ALIGN_START);
            m_grid->remove_row(1);
            return false;
        }
    }

    std::stringstream ss;
    ss << make_time(diff_secs - diff_days);
    m_hours->set_label(ss.str());

    return true;
}

class CountdownWindow : public Gtk::ApplicationWindow
{
public:
    CountdownWindow();
    virtual ~CountdownWindow() = default;

private:
    Gtk::Box *m_box;
    Gtk::Stack *m_stack;
    Gtk::StackSwitcher *m_stack_switcher;

private:
//    void on_event_change();
};

/*
void
CountdownWindow::on_event_change()
{
    auto event = property_event.get_value();
    auto name = event->property_name.get_value();

    update();
}
*/
CountdownWindow::CountdownWindow() :
    Glib::ObjectBase("CountdownWindow"),
    m_box(Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL))),
    m_stack(Gtk::manage(new Gtk::Stack())),
    m_stack_switcher(Gtk::manage(new Gtk::StackSwitcher()))
{
    set_default_size(800, 800);
    m_box->add(*m_stack_switcher);
    m_box->add(*m_stack);
    m_stack_switcher->set_stack(*m_stack);
    m_stack_switcher->set_halign(Gtk::ALIGN_CENTER);
    add(*m_box);
    m_box->show_all();

    m_stack->set_homogeneous(true);
    m_stack->property_visible_child().signal_changed().connect(sigc::track_obj([this](){
        auto event = static_cast<CountdownGrid*>(m_stack->property_visible_child().get_value())->m_event;
        auto style_class = event->m_style_class;
        auto context = m_box->get_style_context();
        for (auto &e : events) {
            context->remove_class(Glib::ustring(std::begin(e.style_class), std::end(e.style_class)));
        }
        context->add_class(style_class);
        set_title(Glib::ustring::compose("%1 Countdown", event->property_name.get_value()));
    }, *this));

    for (auto &e : events) {
        auto event = Event::create(e);
        auto grid = Gtk::manage(new CountdownGrid(event));
        m_stack->add(*grid, event->property_name.get_value(), event->property_name.get_value());
        grid->update();
        Glib::signal_timeout().connect_seconds(sigc::mem_fun(*grid, &CountdownGrid::update), 1);
    }

}

int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create("dance._39music.MikuExpoCountdown");

  app->signal_activate().connect([app]() {
      auto win = new CountdownWindow();
      app->add_window(*win);
      win->show();
      win->set_name("mainwin");

      auto provider = Gtk::CssProvider::create();
      provider->load_from_resource("/org/talinet/mikuexpocountdown/style.css");
      Gtk::StyleContext::add_provider_for_screen(win->get_screen(),
                                                 provider,
                                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  });


  return app->run(argc, argv);
}

#include <chrono>
#include <iostream>
#include <iomanip>
#include <gtkmm.h>
#include "date/date.h"
#include "date/tz.h"

using namespace date::literals;
using namespace std::chrono_literals;

class CountdownWindow : public Gtk::Window
{
public:
    CountdownWindow();

    Gtk::Grid *grid;
    Gtk::Label *m_days;
    Gtk::Label *m_hours;
    Gtk::Label *colon_hm;
    Gtk::Label *colon_ms;

    sigc::connection timeout;
    const date::zoned_time<std::chrono::duration<long>> expoday_jp;
    const std::chrono::time_point<date::local_t, std::chrono::duration<long>> expoday;

    void update();
};

void CountdownWindow::update()
{
    auto now = date::make_zoned(date::current_zone(), std::chrono::system_clock::now()).get_local_time();;
    auto diff = expoday - now;
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
    auto mins = secs / 60;
    auto hours = mins / 60;
    auto days = hours / 24;
    secs = secs % 60;
    mins = mins % 60;
    hours = hours % 24;
    {
        std::stringstream ss;
        ss << days << " days";
        m_days->set_label(ss.str());
    }

    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << hours << ":";
        ss << std::setw(2) << std::setfill('0') << mins << ":";
        ss << std::setw(2) << std::setfill('0') << secs;
        m_hours->set_label(ss.str());
    }
}

CountdownWindow::CountdownWindow() :
    grid(Gtk::manage(new Gtk::Grid())),
    m_days(Gtk::manage(new Gtk::Label())),
    m_hours(Gtk::manage(new Gtk::Label())),
    expoday_jp(date::make_zoned("Asia/Tokyo", date::local_days{6_d/6/2021} + 10h)),
    expoday(date::make_zoned(date::current_zone(), expoday_jp).get_local_time())
{
    set_title("Miku Expo Online Countdown");
    set_default_size(400, 500);

    m_days->set_name("days");
    m_hours->set_name("hours");

    grid->attach(*m_days, 0, 0);
    grid->attach(*m_hours, 0, 1);
    grid->set_halign(Gtk::ALIGN_CENTER);
    grid->set_valign(Gtk::ALIGN_CENTER);
    add(*grid);

    update();

    grid->show_all();

    timeout = Glib::signal_timeout().connect_seconds([this] { update(); return true; }, 1);
}

int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create("org.talinet.mikuexpo.countdown");

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

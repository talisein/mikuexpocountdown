#include <chrono>
#include <iostream>
#include <iomanip>
#include <gtkmm.h>

// until __cpp_lib_chrono >= 201907
#include "date/date.h"
#include "date/tz.h"

namespace {
    using namespace date::literals;
    using namespace std::chrono_literals;

    constexpr auto expo_day = date::local_days{6_d/date::June/2021} + 10h;
    const auto expo_time = date::make_zoned("Asia/Tokyo", expo_day).get_sys_time();
}

class CountdownWindow : public Gtk::Window
{
public:
    CountdownWindow();

    Gtk::Grid *m_grid;
    Gtk::Label *m_days;
    Gtk::Label *m_hours;

    sigc::connection m_timeout;

    bool update() const;
};

bool CountdownWindow::update() const
{
    auto now = std::chrono::system_clock::now();
    auto diff = expo_time - now;
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
    auto mins = secs / 60;
    auto hours = mins / 60;
    auto days = hours / 24;

    if (days > 0) {
        std::stringstream ss;
        ss << days << " day";
        if (days > 1) {
            ss << 's';
        }
        m_days->set_label(ss.str());
    } else {
        if (secs > 0) {
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
    if (hours > 0) {
        ss << std::setw(1) << std::setfill('0') << hours % 24 << ":";
    }
    if (mins > 0) {
        ss << std::setw(2) << std::setfill('0') << mins % 60 << ":";
    }
    if (secs > 0) {
        if (secs > 10) {
            ss << std::setw(2);
        } else {
            ss << std::setw(1);
        }
        ss << std::setfill('0') << secs % 60;
        m_hours->set_label(ss.str());
    }

    return true;
}

CountdownWindow::CountdownWindow() :
    m_grid(Gtk::manage(new Gtk::Grid())),
    m_days(Gtk::manage(new Gtk::Label())),
    m_hours(Gtk::manage(new Gtk::Label()))
{
    set_title("Miku Expo Online Countdown");
    set_default_size(400, 500);

    m_days->set_name("days");
    m_hours->set_name("hours");

    m_grid->attach(*m_days, 0, 0);
    m_grid->attach(*m_hours, 0, 1);
    m_grid->set_halign(Gtk::ALIGN_CENTER);
    m_grid->set_valign(Gtk::ALIGN_CENTER);
    add(*m_grid);

    m_grid->show_all();

    update();

    m_timeout = Glib::signal_timeout().connect_seconds([this] { return update(); }, 1);
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

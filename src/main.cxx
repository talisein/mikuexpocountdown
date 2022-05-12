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

//    constexpr auto ew_local_time = date::local_days{November/19/2021} + 1h;
    constexpr auto mer_local_time = local_days{June/5/2022} + 10h + 30min;
    const auto mer_sys_time = make_zoned("Asia/Tokyo", mer_local_time).get_sys_time();
//    const auto ew_sys_time = date::make_zoned("America/Los_Angeles", ew_local_time).get_sys_time();
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
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono;

    auto diff_secs = floor<seconds>(mer_sys_time - system_clock::now());
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

CountdownWindow::CountdownWindow() :
    m_grid(Gtk::manage(new Gtk::Grid())),
    m_days(Gtk::manage(new Gtk::Label())),
    m_hours(Gtk::manage(new Gtk::Label()))
{
    set_title("Miku Expo Countdown");
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
  auto app = Gtk::Application::create("dance._39music.MikuExpoCountdown");
/*
  std::cout << "Sys time: " << mer_sys_time << std::endl;
  std::cout << "Sys time: " << date::utc_clock::from_sys(mer_sys_time) << std::endl;
  std::cout << "Tokyo: " << date::make_zoned("Asia/Tokyo", mer_sys_time) << std::endl;
  std::cout << "LA: " << date::make_zoned("America/Los_Angeles", mer_sys_time) << std::endl;
  std::cout << "LA Sleep time: " << date::make_zoned("America/Los_Angeles", mer_sys_time - 8h) << std::endl;
  std::cout << "MST: " << date::make_zoned("America/Denver", mer_sys_time) << std::endl;
  std::cout << "New York: " << date::make_zoned("America/New_York", mer_sys_time) << std::endl;
  std::cout << "Mirai Unix Time: " << mer_sys_time.time_since_epoch() << std::endl;
  std::cout << "Mirai UTC: " << date::utc_clock::from_sys(mer_sys_time).time_since_epoch() << std::endl;
*/
//  return 0;
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

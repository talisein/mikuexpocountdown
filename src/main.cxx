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
#include "window.h"

int main (int argc, char *argv[])
{
    auto app = MikuApplication::create("dance._39music.MikuExpoCountdown");

    CountdownWindow *m_run_window = nullptr;
    app->signal_activate().connect([&app, &m_run_window] () {
        if (!m_run_window) {
            m_run_window = new CountdownWindow(app);
            app->add_window(*m_run_window->get_window());
            m_run_window->present();
        } else {
            m_run_window->present();
        }
    });

    app->signal_window_removed().connect([&m_run_window] (Gtk::Window* window) {
        if (window == m_run_window->get_window())
        {
            if (window && window->gobj())
                m_run_window->destroy();

            if (m_run_window) {
                delete m_run_window;
                m_run_window = nullptr;
            }
        }
    });

    app->run(argc, argv);
}

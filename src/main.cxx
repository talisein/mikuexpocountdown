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

#pragma once

#include <ranges>
#include <gtkmm.h>

#include "grid.h"
#include "events.h"

class CountdownWindow : public sigc::trackable
{
public:
    CountdownWindow(const Glib::RefPtr<Gtk::Application>& app);

    void present() {
        m_appwindow->present();
    }

    void destroy() {
        disconnect_signals();
        m_appwindow->destroy();
    }

    Gtk::Window* get_window() const {
        return static_cast<Gtk::Window*>(m_appwindow);
    }

private:
    Gtk::ApplicationWindow *m_appwindow;
    Gtk::Widget *m_tab_overview;
    Gtk::Widget *m_header_bar;
    Gtk::Widget *m_tab_button;
    Gtk::Widget *m_tab_view;
    Gtk::Box *m_box;
    gulong signal_handle_selected_page;

    void disconnect_signals();
};

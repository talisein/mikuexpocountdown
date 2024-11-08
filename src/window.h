#pragma once

#include <ranges>
#include <gtkmm.h>

#include "grid.h"
#include "events.h"
#include "adw_appwindow.h"
#include "adw_tabview.h"

class CountdownWindow : public Adw::ApplicationWindow
{
public:
    CountdownWindow(const Glib::RefPtr<Gtk::Application>& app);

private:
    Gtk::Widget *m_tab_overview;
    Gtk::Widget *m_header_bar;
    Gtk::Widget *m_window_title;
    Gtk::Widget *m_tab_button;
    Adw::TabView *m_tab_view;
    Gtk::Box *m_box;
    Glib::RefPtr<Gio::Settings> m_settings;
    std::optional<AdwTabPage*> map_from_name_to_page(const Glib::ustring& name);
    std::optional<Glib::ustring> map_from_page_to_name(AdwTabPage* page);
};

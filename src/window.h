#pragma once
#include <vector>
#include <peel/class.h>
#include <peel/Adw/ApplicationWindow.h>
#include <peel/Adw/TabView.h>
#include <peel/Adw/TabPage.h>
#include <peel/Adw/TabOverview.h>
#include <peel/Adw/HeaderBar.h>
#include <peel/Adw/WindowTitle.h>
#include <peel/Adw/TabButton.h>
#include <peel/Adw/Application.h>
#include <peel/Gtk/Box.h>
#include <peel/Gio/Settings.h>
#include "grid.h"
#include "events.h"
#include "search_provider.h"

class CountdownWindow final : public peel::Adw::ApplicationWindow
{
    PEEL_SIMPLE_CLASS(CountdownWindow, peel::Adw::ApplicationWindow);

    struct Members {
        peel::Adw::TabOverview  *m_tab_overview = nullptr;
        peel::Adw::HeaderBar    *m_header_bar   = nullptr;
        peel::Adw::WindowTitle  *m_window_title = nullptr;
        peel::Adw::TabButton    *m_tab_button   = nullptr;
        peel::Adw::TabView      *m_tab_view     = nullptr;
        peel::Gtk::Box          *m_box          = nullptr;
        peel::RefPtr<peel::Gio::Settings> m_settings;
        peel::SignalConnection notify_selected_page_connection;
        peel::SignalConnection search_provider_activate_connection;
        std::vector<peel::SignalConnection> expired_connections;
    } *m = nullptr;

    void init(Class *);
    void vfunc_dispose();
    ~CountdownWindow() { delete m; }

    peel::Adw::TabPage *map_from_name_to_page(const char *name);
    const char         *map_from_page_to_name(peel::Adw::TabPage *page);

    static gboolean settings_get_selected_page(GValue *, GVariant *, gpointer);
    static GVariant *settings_set_selected_page(const GValue *, const GVariantType *, gpointer);

public:
    void activate_page(const char *identifier);
    void connect_search_provider(SearchProvider &sp);

    static CountdownWindow *create(peel::Adw::Application *app) {
        return peel::GObject::Object::create<CountdownWindow>(
            peel::Gtk::Window::prop_application(), app);
    }
};

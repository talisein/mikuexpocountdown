#pragma once
#include <vector>
#include <peel/class.h>
#include <peel/widget-template.h>
#include <peel/Adw/ApplicationWindow.h>
#include <peel/Adw/TabView.h>
#include <peel/Adw/TabPage.h>
#include <peel/Adw/WindowTitle.h>
#include <peel/Adw/Application.h>
#include <peel/Gio/Settings.h>
#include "grid.hpp"
#include "events.hpp"
#include "search_provider.hpp"

class CountdownWindow final : public peel::Adw::ApplicationWindow
{
    PEEL_SIMPLE_CLASS(CountdownWindow, peel::Adw::ApplicationWindow);

    struct Members {
        peel::Adw::WindowTitle  *window_title = nullptr;
        peel::Adw::TabView      *tab_view     = nullptr;
        peel::RefPtr<peel::Gio::Settings> settings;
        peel::SignalConnection search_provider_activate_connection;
        std::vector<peel::SignalConnection> expired_connections;
    } m;

    void init(Class *);
    void vfunc_dispose();

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

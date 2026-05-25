#include <ranges>
#include <format>
#include <peel/Gtk/Orientation.h>
#include <peel/Gio/SettingsBindFlags.h>
#include <peel/GLib/Variant.h>
#include <peel/GObject/Object.h>
#include "window.h"
#include "util.hpp"

PEEL_CLASS_IMPL(CountdownWindow, "MikuCountdownWindow", peel::Adw::ApplicationWindow)

void CountdownWindow::Class::init() {
    override_vfunc_dispose<CountdownWindow>();
}

void CountdownWindow::init(Class *) {
    m = new Members;

    auto overview_fp    = peel::Adw::TabOverview::create();  m->m_tab_overview = overview_fp;
    auto hbar_fp        = peel::Adw::HeaderBar::create();     m->m_header_bar   = hbar_fp;
    auto title_fp       = peel::Adw::WindowTitle::create("", ""); m->m_window_title = title_fp;
    auto tabbutton_fp   = peel::Adw::TabButton::create();     m->m_tab_button   = tabbutton_fp;
    auto tabview_fp     = peel::Adw::TabView::create();       m->m_tab_view     = tabview_fp;
    auto box_fp         = peel::Gtk::Box::create(peel::Gtk::Orientation::VERTICAL, 0); m->m_box = box_fp;

    set_name("mainwin");

    m->m_header_bar->set_title_widget(std::move(title_fp));

    m->notify_selected_page_connection = peel::SignalConnection(
        m->m_tab_view->connect_notify(
            peel::Adw::TabView::prop_selected_page(),
            [this](peel::GObject::Object *, peel::GObject::ParamSpec *) {
                auto *page = m->m_tab_view->get_selected_page();
                if (!page) return;
                auto *child = page->get_child()->cast<CountdownGrid>();
                m->m_window_title->set_title(format_to_peel("{} Countdown", child->event_name()));
            }));

    m->m_header_bar->pack_end(std::move(tabbutton_fp));
    m->m_tab_button->set_view(m->m_tab_view);
    m->m_tab_button->set_action_name("overview.open");

    m->m_box->append(std::move(hbar_fp));
    m->m_box->append(std::move(tabview_fp));

    m->m_tab_overview->set_child(std::move(box_fp));
    m->m_tab_overview->set_view(m->m_tab_view);
    m->m_tab_view->set_hexpand(true);
    m->m_tab_view->set_vexpand(true);

    set_content(std::move(overview_fp));

    using namespace std::ranges;
    for (const auto &e : Miku::get_events() | views::filter(&Miku::Event::is_unexpired)) {
        auto *grid = CountdownGrid::create(e);
        auto *page = m->m_tab_view->append_pinned(peel::FloatPtr<peel::Gtk::Widget>(grid));
        page->set_title(e->name());
        m->expired_connections.emplace_back(grid->connect_expired([this, page](CountdownGrid *) {
            m->m_tab_view->close_page(page);
        }));
    }

    m->m_settings = peel::Gio::Settings::create("dance._39music.MikuExpoCountdown.State");
    m->m_settings->bind("width",         this, "default-width",  peel::Gio::Settings::BindFlags::DEFAULT);
    m->m_settings->bind("height",        this, "default-height", peel::Gio::Settings::BindFlags::DEFAULT);
    m->m_settings->bind("is-maximized",  this, "maximized",      peel::Gio::Settings::BindFlags::DEFAULT);
    m->m_settings->bind("is-fullscreen", this, "fullscreened",   peel::Gio::Settings::BindFlags::DEFAULT);

    g_settings_bind_with_mapping(
        reinterpret_cast<GSettings *>(static_cast<peel::Gio::Settings *>(m->m_settings)),
        "selected-page",
        reinterpret_cast<GObject *>(m->m_tab_view),
        "selected-page",
        G_SETTINGS_BIND_DEFAULT,
        &CountdownWindow::settings_get_selected_page,
        &CountdownWindow::settings_set_selected_page,
        this, nullptr);
}

void CountdownWindow::vfunc_dispose() {
    if (m) {
        m->notify_selected_page_connection.disconnect();
        m->search_provider_activate_connection.disconnect();
        m->expired_connections.clear();
        if (m->m_settings) {
            peel::Gio::Settings::unbind(this, "selected-page");
            m->m_settings = nullptr;
        }
    }
    parent_vfunc_dispose<CountdownWindow>();
}

peel::Adw::TabPage *CountdownWindow::map_from_name_to_page(const char *name) {
    const int n = m->m_tab_view->get_n_pages();
    for (int i = 0; i < n; ++i) {
        auto *page = m->m_tab_view->get_nth_page(i);
        if (std::string_view(page->get_title()) == std::string_view(name))
            return page;
    }
    return nullptr;
}

const char *CountdownWindow::map_from_page_to_name(peel::Adw::TabPage *page) {
    return page ? page->get_title() : nullptr;
}

gboolean CountdownWindow::settings_get_selected_page(GValue *value, GVariant *variant, gpointer user_data) {
    auto *self = static_cast<CountdownWindow *>(user_data);
    const char *name = reinterpret_cast<peel::GLib::Variant*>(variant)->get<const char*>();
    auto *page = self->map_from_name_to_page(name);
    if (page) {
        peel::GObject::Value::Traits<peel::Adw::TabPage>::set(value, page);
        return TRUE;
    }
    return FALSE;
}

GVariant *CountdownWindow::settings_set_selected_page(const GValue *value, const GVariantType *, gpointer user_data) {
    auto *self = static_cast<CountdownWindow *>(user_data);
    auto *page = peel::GObject::Value::Traits<peel::Adw::TabPage>::get(value);
    const char *name = self->map_from_page_to_name(page);
    return name ? reinterpret_cast<GVariant*>(
        peel::GLib::Variant::create<const char*>(name).release_floating_ptr()) : nullptr;
}

void CountdownWindow::activate_page(const char *identifier) {
    auto *page = map_from_name_to_page(identifier);
    if (page)
        m->m_tab_view->set_selected_page(page);
}

void CountdownWindow::connect_search_provider(SearchProvider &sp) {
    m->search_provider_activate_connection = peel::SignalConnection(
        sp.connect_activate([this](SearchProvider *, const char *name) {
            activate_page(name);
        }));
}

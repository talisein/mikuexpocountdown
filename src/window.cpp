#include <ranges>
#include <format>
#include <peel/Gio/SettingsBindFlags.h>
#include <peel/GLib/Variant.h>
#include <peel/GObject/Object.h>
#include <peel/GObject/BindingFlags.h>
#include "window.hpp"
#include "util.hpp"

PEEL_CLASS_IMPL(CountdownWindow, "MikuCountdownWindow", peel::Adw::ApplicationWindow)

void CountdownWindow::Class::init() {
    override_vfunc_dispose<CountdownWindow>();
    set_template_from_resource("/dance/_39music/MikuExpoCountdown/miku-countdown-window.ui");
    PEEL_WIDGET_TEMPLATE_BIND_CHILD(CountdownWindow, m.window_title, "window-title");
    PEEL_WIDGET_TEMPLATE_BIND_CHILD(CountdownWindow, m.tab_view,     "tab-view");
}

void CountdownWindow::init(Class *) {
    new (&m) Members;
    init_template();

    peel::GObject::Object::bind_property(
        m.tab_view,    peel::Adw::TabView::prop_selected_page(),
        m.window_title, peel::Adw::WindowTitle::prop_title(),
        peel::GObject::BindingFlags::SYNC_CREATE,
        [](peel::Adw::TabPage *page) -> peel::String {
            if (!page) return {};
            auto *child = page->get_child()->cast<CountdownGrid>();
            if (!child) return {};
            return format_to_peel("{} Countdown", child->get_property(CountdownGrid::prop_event_name()));
        });

    using namespace std::ranges;
    for (const auto &e : Miku::get_events() | views::filter(&Miku::Event::is_unexpired)) {
        auto *grid = CountdownGrid::create(e);
        auto *page = m.tab_view->append_pinned(peel::FloatPtr<peel::Gtk::Widget>(grid));
        peel::GObject::Object::bind_property(
            e, Miku::Event::prop_name(),
            page, peel::Adw::TabPage::prop_title(),
            peel::GObject::BindingFlags::SYNC_CREATE);
        m.expired_connections.emplace_back(grid->connect_expired([this, page](CountdownGrid *) {
            m.tab_view->close_page(page);
        }));
    }

    m.settings = peel::Gio::Settings::create("dance._39music.MikuExpoCountdown.State");
    m.settings->bind("width",         this, "default-width",  peel::Gio::Settings::BindFlags::DEFAULT);
    m.settings->bind("height",        this, "default-height", peel::Gio::Settings::BindFlags::DEFAULT);
    m.settings->bind("is-maximized",  this, "maximized",      peel::Gio::Settings::BindFlags::DEFAULT);
    m.settings->bind("is-fullscreen", this, "fullscreened",   peel::Gio::Settings::BindFlags::DEFAULT);

    g_settings_bind_with_mapping(
        reinterpret_cast<GSettings *>(static_cast<peel::Gio::Settings *>(m.settings)),
        "selected-page",
        reinterpret_cast<GObject *>(m.tab_view),
        "selected-page",
        G_SETTINGS_BIND_DEFAULT,
        &CountdownWindow::settings_get_selected_page,
        &CountdownWindow::settings_set_selected_page,
        this, nullptr);
}

void CountdownWindow::vfunc_dispose() {
    m.search_provider_activate_connection.disconnect();
    m.expired_connections.clear();
    if (m.settings) {
        peel::Gio::Settings::unbind(this, "selected-page");
        m.settings = nullptr;
    }
    dispose_template(peel::GObject::Type::of<CountdownWindow>());
    parent_vfunc_dispose<CountdownWindow>();
}

peel::Adw::TabPage *CountdownWindow::map_from_name_to_page(const char *name) {
    const int n = m.tab_view->get_n_pages();
    for (int i = 0; i < n; ++i) {
        auto *page = m.tab_view->get_nth_page(i);
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
        m.tab_view->set_selected_page(page);
}

void CountdownWindow::connect_search_provider(SearchProvider &sp) {
    m.search_provider_activate_connection = peel::SignalConnection(
        sp.connect_activate([this](SearchProvider *, const char *name) {
            activate_page(name);
        }));
}

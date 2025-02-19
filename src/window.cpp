#include <iostream>
#include "application.h"
#include "window.h"
#include "adwaita.h"

CountdownWindow::CountdownWindow(const Glib::RefPtr<Gtk::Application>& app) :
    Glib::ObjectBase("CountdownWindow"),
    Adw::ApplicationWindow(app),
    m_tab_overview (Glib::wrap(adw_tab_overview_new())),
    m_header_bar   (Glib::wrap(adw_header_bar_new())),
    m_window_title (Glib::wrap(adw_window_title_new(nullptr, nullptr))),
    m_tab_button   (Glib::wrap(adw_tab_button_new())),
    m_tab_view     (Gtk::make_managed<Adw::TabView>()),
    m_box          (Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL)),
    m_settings(Gio::Settings::create("dance._39music.MikuExpoCountdown.State"))
{
    AdwTabOverview *tab_overview = ADW_TAB_OVERVIEW (m_tab_overview->gobj());
    AdwHeaderBar   *header_bar   = ADW_HEADER_BAR   (m_header_bar->gobj());
    AdwTabButton   *tab_button   = ADW_TAB_BUTTON   (m_tab_button->gobj());
    AdwWindowTitle *window_title = ADW_WINDOW_TITLE (m_window_title->gobj());

    set_name("mainwin");

    adw_header_bar_set_title_widget(header_bar, m_window_title->gobj());
    m_tab_view->property_selected_page().signal_changed().connect([window_title, tab_view = m_tab_view->gobj()]() {
        AdwTabPage *page = adw_tab_view_get_selected_page(tab_view);
        CountdownGrid *grid = static_cast<CountdownGrid*>(Glib::wrap(GTK_CENTER_BOX(adw_tab_page_get_child(page))));
        auto s = Glib::ustring::compose("%1 Countdown", grid->m_event->name);
        adw_window_title_set_title(window_title, s.c_str());
    });

    adw_header_bar_pack_end(header_bar, GTK_WIDGET(tab_button));
    adw_tab_button_set_view(tab_button, m_tab_view->gobj());
    gtk_actionable_set_action_name(GTK_ACTIONABLE(tab_button), "overview.open");
    m_box->append(*m_header_bar);
    m_box->append(*m_tab_view);

    set_content(*m_tab_overview);
    adw_tab_overview_set_child(tab_overview, static_cast<Gtk::Widget*>(m_box)->gobj());
    adw_tab_overview_set_view(tab_overview, m_tab_view->gobj());
    m_tab_view->set_expand(true);

    using namespace std::ranges;
    for (const auto &e : Miku::get_events() | views::filter(&Miku::Event::is_unexpired)) {
        auto grid = Gtk::make_managed<CountdownGrid>(e);
        AdwTabPage *page = m_tab_view->append(*grid);
        adw_tab_page_set_title(page, e->name.c_str());
        adw_tab_view_set_page_pinned(m_tab_view->gobj(), page, true);
        grid->signal_expired().connect(sigc::bind(sigc::mem_fun(*m_tab_view, &Adw::TabView::close_page),
                                                  page));

    }

    m_settings->bind("width", this, "default-width");
    m_settings->bind("height", this, "default-height");
    m_settings->bind("is-maximized", this, "maximized");
    m_settings->bind("is-fullscreen", this, "fullscreened");
    m_settings->bind<Glib::ustring, AdwTabPage*>("selected-page", m_tab_view, "selected-page", Gio::Settings::BindFlags::DEFAULT,
                                                 sigc::mem_fun(*this, &CountdownWindow::map_from_name_to_page),
                                                 sigc::mem_fun(*this, &CountdownWindow::map_from_page_to_name));


    if (auto miku_app = std::dynamic_pointer_cast<MikuApplication>(app); miku_app) {
        search_provider_activate_connection = miku_app->get_search_provider().signal_activate.connect(sigc::mem_fun(*this, &CountdownWindow::activate_page));
    }
}

std::optional<AdwTabPage*>
CountdownWindow::map_from_name_to_page(const Glib::ustring& name)
{
    GListModel* list = G_LIST_MODEL(adw_tab_view_get_pages(m_tab_view->gobj()));
    const gsize list_size = g_list_model_get_n_items(list);
    for (gsize i = 0; i < list_size; ++i) {
        auto tab_page = ADW_TAB_PAGE(g_list_model_get_item(list, i));
        auto title = Glib::convert_const_gchar_ptr_to_ustring(adw_tab_page_get_title(tab_page));
        if (title == name)
            return tab_page;
    }

    return std::nullopt;
}

std::optional<Glib::ustring>
CountdownWindow::map_from_page_to_name(AdwTabPage* page)
{
    return Glib::convert_const_gchar_ptr_to_ustring(adw_tab_page_get_title(page));
}

void
CountdownWindow::activate_page(Glib::ustring identifier)
{
    auto page = map_from_name_to_page(identifier);
    if (page.has_value()) {
        m_tab_view->set_selected_page(page.value());
    }
}

#include "window.h"
#include "adwaita.h"


namespace {
    extern "C" {
        static void on_selected_page (GObject* self,
                               GParamSpec*,
                               gpointer user_data)
        {
            AdwTabView     *tabview   = ADW_TAB_VIEW(self);
            AdwWindowTitle *title     = ADW_WINDOW_TITLE(user_data);
            AdwTabPage     *page      = adw_tab_view_get_selected_page(tabview);
            CountdownGrid  *grid      = static_cast<CountdownGrid*>(Glib::wrap(GTK_CENTER_BOX(adw_tab_page_get_child(page))));

            auto s = Glib::ustring::compose("Miku Expo %1 Countdown", grid->m_event->name);
            adw_window_title_set_title(title, s.c_str());
        }
    }
}

void CountdownWindow::disconnect_signals()
{
    if (m_tab_view) {
        g_signal_handler_disconnect(m_tab_view->gobj(), signal_handle_selected_page);
        m_tab_view = nullptr;
    }
}

CountdownWindow::CountdownWindow(const Glib::RefPtr<Gtk::Application>& app) :
    m_appwindow    (Glib::wrap(GTK_APPLICATION_WINDOW(adw_application_window_new(app->gobj())))),
    m_tab_overview (Glib::wrap(adw_tab_overview_new())),
    m_header_bar   (Glib::wrap(adw_header_bar_new())),
    m_tab_button   (Glib::wrap(adw_tab_button_new())),
    m_tab_view     (Glib::wrap(GTK_WIDGET(adw_tab_view_new()))),
    m_box          (Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL))
{
    AdwApplicationWindow *appwindow = ADW_APPLICATION_WINDOW (m_appwindow->gobj());
    AdwTabOverview       *overview  = ADW_TAB_OVERVIEW       (m_tab_overview->gobj());
    AdwHeaderBar         *headerbar = ADW_HEADER_BAR         (m_header_bar->gobj());
    AdwTabButton         *tabbutton = ADW_TAB_BUTTON         (m_tab_button->gobj());
    AdwTabView           *tabview   = ADW_TAB_VIEW           (m_tab_view->gobj());

    m_appwindow->set_name("mainwin");

    auto window_title = adw_window_title_new(NULL, NULL);
    adw_header_bar_set_title_widget(headerbar, window_title);
    signal_handle_selected_page = g_signal_connect(tabview, "notify::selected-page", G_CALLBACK(on_selected_page), window_title);
    adw_header_bar_pack_end(headerbar, GTK_WIDGET(tabbutton));
    adw_tab_button_set_view(tabbutton, tabview);
    gtk_actionable_set_action_name(GTK_ACTIONABLE(tabbutton), "overview.open");
    m_box->append(*m_header_bar);
    m_box->append(*m_tab_view);

    auto provider = Gtk::CssProvider::create();
    provider->load_from_resource("/org/talinet/mikuexpocountdown/style.css");
    auto style_context = m_appwindow->get_style_context();
    for (auto &display : Gdk::DisplayManager::get()->list_displays()) {
        style_context->add_provider_for_display(display, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    adw_application_window_set_content(appwindow, m_tab_overview->gobj());
    adw_tab_overview_set_child(overview, static_cast<Gtk::Widget*>(m_box)->gobj());
    adw_tab_overview_set_view(overview, tabview);
    m_tab_view->set_expand(true);

    using namespace std::ranges;
    for (const auto &e : Miku::get_events() | views::filter(&Miku::Event::is_unexpired)) {
        auto grid = Gtk::make_managed<CountdownGrid>(e);
        AdwTabPage *page = adw_tab_view_append(tabview, static_cast<Gtk::Widget*>(grid)->gobj());
        adw_tab_page_set_title(page, e->name.c_str());

        grid->signal_expired().connect([tabview, page]() {
            adw_tab_view_close_page(tabview, page);
        });
    }
}

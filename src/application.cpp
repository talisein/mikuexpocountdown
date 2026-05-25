#include "application.h"
#include "window.h"

PEEL_CLASS_IMPL(MikuApplication, "MikuApplication", peel::Adw::Application)

void MikuApplication::Class::init() {
    override_vfunc_dispose<MikuApplication>();
    override_vfunc_activate<MikuApplication>();
    override_vfunc_dbus_register<MikuApplication>();
    override_vfunc_dbus_unregister<MikuApplication>();
}

void MikuApplication::init(Class *) {
    m = new Members;
    m->search = SearchProvider::create();
    m->search_provider_activate_connection = peel::SignalConnection(
        m->search->connect_activate([this](SearchProvider *sp, const char *name) {
            search_activation(sp, name);
        }));
}

void MikuApplication::vfunc_dispose() {
    if (m) {
        m->search_provider_activate_connection.disconnect();
        m->search = nullptr;
    }
    parent_vfunc_dispose<MikuApplication>();
}

void MikuApplication::vfunc_activate() {
    parent_vfunc_activate<MikuApplication>();
    m->search_provider_activate_connection.disconnect();

    auto *active = cast<peel::Gtk::Application>()->get_active_window();
    if (active) {
        active->present();
        return;
    }

    auto *window = CountdownWindow::create(cast<peel::Adw::Application>());
    window->connect_search_provider(*m->search);
    window->present();
}

bool MikuApplication::vfunc_dbus_register(peel::Gio::DBusConnection *conn,
                                           const char *object_path,
                                           peel::UniquePtr<peel::GLib::Error> *err) {
    auto res = parent_vfunc_dbus_register<MikuApplication>(conn, object_path, err);
    m->search->export_(conn, "/dance/_39music/MikuExpoCountdown/SearchProvider", nullptr);
    return res;
}

void MikuApplication::vfunc_dbus_unregister(peel::Gio::DBusConnection *conn,
                                             const char *object_path) {
    m->search->unexport();
    parent_vfunc_dbus_unregister<MikuApplication>(conn, object_path);
}

void MikuApplication::search_activation(SearchProvider *, const char *name) {
    cast<peel::Gio::Application>()->activate();
    auto *active = cast<peel::Gtk::Application>()->get_active_window();
    if (auto *cw = active ? active->cast<CountdownWindow>() : nullptr)
        cw->activate_page(name);
}

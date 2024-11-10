#pragma once
#include <gtkmm.h>
#include "search_provider.h"
#include "adw_application.h"

class MikuApplication final : public Adw::Application
{
public:
    MikuApplication(const Glib::ustring &application_id);

    virtual ~MikuApplication() override final = default;

    template<typename... Args>
    static Glib::RefPtr<MikuApplication>
    create(Args&&... args)
    {
        return std::make_shared<MikuApplication>(std::forward<Args&&...>(args...));
    }


    SearchProvider& get_search_provider() { return search; };

protected:

    virtual bool
    dbus_register_vfunc (const Glib::RefPtr<Gio::DBus::Connection> &connection, const Glib::ustring &object_path) override final
    {
        auto res = Gtk::Application::dbus_register_vfunc(connection, object_path);
        search.register_object(connection, "/dance/_39music/MikuExpoCountdown/SearchProvider");
        return res;
    }

    virtual void
    dbus_unregister_vfunc (const Glib::RefPtr<Gio::DBus::Connection> &connection, const Glib::ustring &object_path) override final
    {
        search.unregister_object();
        Gtk::Application::dbus_unregister_vfunc(connection, object_path);
    }

private:
    SearchProvider search;

    void search_activation(const Glib::ustring& name);
    void on_activate();

    sigc::scoped_connection search_provider_activate_connection;
    sigc::scoped_connection signal_activate_connection;
};

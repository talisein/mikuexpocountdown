#pragma once
#include <peel/class.h>
#include <peel/Adw/Application.h>
#include <peel/Gio/Application.h>
#include <peel/Gio/ApplicationFlags.h>
#include <peel/Gio/DBusConnection.h>
#include <peel/UniquePtr.h>
#include <peel/GLib/Error.h>
#include "search_provider.h"

class MikuApplication final : public peel::Adw::Application
{
    friend class peel::Gio::Application;
    PEEL_SIMPLE_CLASS(MikuApplication, peel::Adw::Application);

    struct Members {
        peel::RefPtr<SearchProvider>  search;
        peel::SignalConnection        search_provider_activate_connection;
    } *m = nullptr;

    void init(Class *);
    void vfunc_dispose();

    void vfunc_activate();
    bool vfunc_dbus_register(peel::Gio::DBusConnection *, const char *,
                             peel::UniquePtr<peel::GLib::Error> *);
    void vfunc_dbus_unregister(peel::Gio::DBusConnection *, const char *);

    void search_activation(SearchProvider *, const char *name);

public:
    SearchProvider &get_search_provider() { return *m->search; }

    static peel::RefPtr<MikuApplication> create() {
        return peel::GObject::Object::create<MikuApplication>(
            prop_application_id(), "dance._39music.MikuExpoCountdown",
            prop_flags(), peel::Gio::Application::Flags::DEFAULT_FLAGS);
    }
};

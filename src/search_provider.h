#pragma once
#include <peel/class.h>
#include <peel/signal.h>
#include <peel/Gio/DBusInterfaceSkeleton.h>
#include <peel/Gio/DBusMethodInvocation.h>
#include "events.h"

class SearchProvider final : public peel::Gio::DBusInterfaceSkeleton
{
    friend class peel::Gio::DBusInterfaceSkeleton;
    PEEL_SIMPLE_CLASS(SearchProvider, peel::Gio::DBusInterfaceSkeleton);

    void init(Class *) {}
    void vfunc_dispose();
    peel::Gio::DBusInterface::Info    *vfunc_get_info();
    peel::Gio::DBusInterface::VTable  *vfunc_get_vtable();

public:
    static peel::Signal<SearchProvider, void(const char *)> s_activate;

    static peel::RefPtr<SearchProvider> create() {
        return peel::GObject::Object::create<SearchProvider>();
    }

    template<typename H>
    peel::SignalConnection::Token connect_activate(H &&h) {
        return s_activate.connect(this, peel::GLib::Quark{}, std::forward<H>(h));
    }
};

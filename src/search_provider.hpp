#pragma once
#include <peel/class.h>
#include <peel/signal.h>
#include <peel/Gio/DBusInterfaceSkeleton.h>
#include <peel/Gio/DBusMethodInvocation.h>
#include "events.hpp"

class SearchProvider final : public peel::Gio::DBusInterfaceSkeleton
{
    friend class peel::Gio::DBusInterfaceSkeleton;
    PEEL_SIMPLE_CLASS(SearchProvider, peel::Gio::DBusInterfaceSkeleton);

    void init(Class *) {}
    void vfunc_dispose();
    peel::Gio::DBusInterface::Info    *vfunc_get_info();
    peel::Gio::DBusInterface::VTable  *vfunc_get_vtable();

public:
    static peel::Signal<SearchProvider, void(const char *)> signal_activate;

    static peel::RefPtr<SearchProvider> create() {
        return peel::GObject::Object::create<SearchProvider>();
    }

    PEEL_SIGNAL_CONNECT_METHOD(activate, signal_activate);
};

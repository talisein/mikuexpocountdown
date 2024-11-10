#include "application.h"
#include "window.h"

MikuApplication::MikuApplication(const Glib::ustring &application_id) :
    Glib::ObjectBase("MikuApplication"),
    Adw::Application(application_id)
{
    search_provider_activate_connection = search.signal_activate.connect(sigc::hide(sigc::mem_fun(*this, &Gio::Application::activate)));
}

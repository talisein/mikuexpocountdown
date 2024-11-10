#include "application.h"
#include "window.h"

MikuApplication::MikuApplication(const Glib::ustring &application_id) :
    Glib::ObjectBase("MikuApplication"),
    Adw::Application(application_id)
{
}

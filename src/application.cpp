#include "application.h"
#include "window.h"

MikuApplication::MikuApplication(const Glib::ustring &application_id) :
    Glib::ObjectBase("MikuApplication"),
    Adw::Application(application_id)
{
    signal_activate().connect(sigc::mem_fun(*this, &MikuApplication::on_activate));
    search_provider_activate_connection = search.signal_activate.connect(sigc::mem_fun(*this, &MikuApplication::search_activation));
}

void
MikuApplication::search_activation(const Glib::ustring& name)
{
    activate();
    if (auto miku_window = dynamic_cast<CountdownWindow*>(get_run_window()); miku_window) {
        miku_window->activate_page(name);
    } else {
        std::cerr << "Couldn't find or cast window after activation\n";
    }
}

void
MikuApplication::on_activate()
{
    search_provider_activate_connection.disconnect();
}

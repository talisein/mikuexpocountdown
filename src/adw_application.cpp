#include "adw_application.h"
#include <gtkmm/init.h>
#include <gtk/gtk.h>

#include <pangomm/wrap_init.h>
#include <gdkmm/wrap_init.h>
#include <gtkmm/wrap_init.h>

namespace
{

    static void init_gtkmm_internals()
    {
        static bool init_done = false;

        if(!init_done)
        {
            Glib::init();
            Gio::init();

            // Populate the map of GTypes to C++ wrap_new() functions.
            Pango::wrap_init();
            Gdk::wrap_init();
            Gtk::wrap_init();

            // Shall gtk_init() set the global C locale to the user's preferred locale?
            if (!Glib::get_init_to_users_preferred_locale())
                gtk_disable_setlocale();

            init_done = true;
        }
    }

// Make sure the C++ locale is the same as the C locale.
// The first call to Glib::init() fixes this. We repeat it here just to be sure.
// The application program may have changed the locale settings after
// the first call to Glib::init(). Unlikely, but possible.
    static void set_cxx_locale_to_c_locale()
    {
        try
        {
            // Make the C++ locale equal to the C locale.
            std::locale::global(std::locale(std::setlocale(LC_ALL, nullptr)));
        }
        catch (const std::runtime_error& ex)
        {
            g_warning("Can't make the global C++ locale equal to the C locale.\n"
                      "   %s\n   C locale = %s\n", ex.what(), std::setlocale(LC_ALL, nullptr));
        }
    }

} // anonymous namespace


namespace Adw
{

    const Glib::Class& Application_Class::init()
    {
        if(!gtype_) // create the GType if necessary
        {
            // Glib::Class has to know the class init function to clone custom types.
            class_init_func_ = &Application_Class::class_init_function;

            // This is actually just optimized away, apparently with no harm.
            // Make sure that the parent type has been created.
            //CppClassParent::CppObjectType::get_type();

            // Create the wrapper type, with the same class/instance size as the base type.
            register_derived_type(adw_application_get_type());

            // Add derived versions of interfaces, if the C type implements any interfaces:

        }

        return *this;
    }


    void Application_Class::class_init_function(void* g_class, void* class_data)
    {
        const auto klass = static_cast<BaseClassType*>(g_class);
        CppClassParent::class_init_function(klass, class_data);

    }

    Glib::ObjectBase* Application_Class::wrap_new(GObject* object)
    {
        return new Application((AdwApplication*)object);
    }

    const Glib::Class& Application::custom_class_init()
    {
        init_gtkmm_internals();
        return application_class_.init();
    }

    Application::Application(const Glib::ustring& application_id, Gio::Application::Flags flags)
        :
        // Mark this class as non-derived to allow C++ vfuncs to be skipped.
        //Note that GApplication complains about "" but allows nullptr, so we avoid passing "".
        Glib::ObjectBase(nullptr),
        Gtk::Application(Glib::ConstructParams(custom_class_init(), "application_id", (application_id.empty() ? nullptr : application_id.c_str()), "flags", GApplicationFlags(flags), nullptr))
    {
        gtk_init();
        set_cxx_locale_to_c_locale();
    }

    Glib::RefPtr<Application> Application::create(const Glib::ustring& application_id, Gio::Application::Flags flags)
    {
        return Glib::RefPtr<Application>( new Application(application_id, flags) );
    }


/* The implementation: */

    AdwApplication* Application::gobj_copy()
    {
        reference();
        return gobj();
    }

    Application::Application(const Glib::ConstructParams& construct_params)
        : Gtk::Application(construct_params)
    {
    }

    Application::Application(AdwApplication* castitem)
        : Gtk::Application((GtkApplication*)(castitem))
    {}


    Application::Application(Application&& src) noexcept
        : Gtk::Application(std::move(src))
    {}

    Application& Application::operator=(Application&& src) noexcept
    {
        Gtk::Application::operator=(std::move(src));
        return *this;
    }


    Application::~Application() noexcept
    {}

    Application::CppClassType Application::application_class_; // initialize static member

    GType Application::get_type()
    {
        return application_class_.init().get_type();
    }


    GType Application::get_base_type()
    {
        return adw_application_get_type();
    }


}

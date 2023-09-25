#include "adw_appwindow.h"

namespace Adw {


    const Glib::Class& ApplicationWindow_Class::init()
    {
        if(!gtype_) // create the GType if necessary
        {
            // Glib::Class has to know the class init function to clone custom types.
            class_init_func_ = &ApplicationWindow_Class::class_init_function;

            // This is actually just optimized away, apparently with no harm.
            // Make sure that the parent type has been created.
            //CppClassParent::CppObjectType::get_type();

            // Create the wrapper type, with the same class/instance size as the base type.
            register_derived_type(adw_application_window_get_type());
        }

        return *this;
    }


    void ApplicationWindow_Class::class_init_function(void* g_class, void* class_data)
    {
        const auto klass = static_cast<BaseClassType*>(g_class);
        CppClassParent::class_init_function(klass, class_data);


    }

    Glib::ObjectBase* ApplicationWindow_Class::wrap_new(GObject* o)
    {
        return new ApplicationWindow((AdwApplicationWindow*)(o)); //top-level windows can not be manage()ed.

    }

    ApplicationWindow::ApplicationWindow(const Glib::RefPtr<Gtk::Application>& application)
        :
        // Mark this class as non-derived to allow C++ vfuncs to be skipped.
        Glib::ObjectBase(nullptr),
        Gtk::ApplicationWindow(Glib::ConstructParams(application_window_class_.init()))
    {
        // Don't set the "application" property in ConstructParams. It would result in
        // an attempt to set two C++ wrappers on the GApplicationWindow C object,
        // if "application" is not a plain Gtk::Application, but derived from it.
        // Like so:
        // - Glib::Object::Object(const Glib::ConstructParams& construct_params) calls
        //   g_object_newv() to create a new GApplicationWindow.
        // - In gtk+, gtk_application_add_window() is called. It emits the window_added
        //   signal with the new GApplicationWindow as a parameter.
        // - Application_signal_window_added_callback() calls Glib::wrap(GWindow*)
        //   before *this has been set as the wrapper. Glib::wrap() then creates a
        //   wrapper (not *this).
        // - Glib::Object::Object() calls Glib::ObjectBase::initialize(), which calls
        //   Glib::ObjectBase::_set_current_wrapper() to set *this as the C++ wrapper,
        //   but by then another wrapper has already been set.
        // https://bugzilla.gnome.org/show_bug.cgi?id=758813

        if (application)
            application->add_window(*this);
    }

    ApplicationWindow::ApplicationWindow(const Glib::ConstructParams& construct_params)
        : Gtk::ApplicationWindow(construct_params)
    {
    }

    ApplicationWindow::ApplicationWindow(AdwApplicationWindow* castitem)
        : Gtk::ApplicationWindow((GtkApplicationWindow*)(castitem))
    {
    }


    ApplicationWindow::ApplicationWindow(ApplicationWindow&& src) noexcept
        : Gtk::ApplicationWindow(std::move(src))
    {}

    ApplicationWindow& ApplicationWindow::operator=(ApplicationWindow&& src) noexcept
    {
        Gtk::ApplicationWindow::operator=(std::move(src));
        return *this;
    }

    ApplicationWindow::~ApplicationWindow() noexcept
    {
        destroy_();
    }

    ApplicationWindow::CppClassType ApplicationWindow::application_window_class_; // initialize static member

    GType ApplicationWindow::get_type()
    {
        return application_window_class_.init().get_type();
    }


    GType ApplicationWindow::get_base_type()
    {
        return adw_application_window_get_type();
    }


    ApplicationWindow::ApplicationWindow()
        :
        // Mark this class as non-derived to allow C++ vfuncs to be skipped.
        Glib::ObjectBase(nullptr),
        Gtk::ApplicationWindow(Glib::ConstructParams(application_window_class_.init()))
    {
    }

    Gtk::Widget* ApplicationWindow::get_content() {
        return Glib::wrap(adw_application_window_get_content(gobj()));
    }

    void ApplicationWindow::set_content(Gtk::Widget& content) {
        adw_application_window_set_content(gobj(), (content).gobj());
    }
}

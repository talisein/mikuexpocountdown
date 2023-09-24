#pragma once
#include <adwaita.h>
#include <glibmm.h>
#include <glibmm/class.h>
#include <gtkmm.h>
#include <giomm.h>

#include <gtkmm/private/application_p.h>

namespace Adw
{
    class Application;

    class Application_Class : public Glib::Class {
    public:
        using CppObjectType = ::Adw::Application;
        using BaseObjectType = AdwApplication;
        using BaseClassType = AdwApplicationClass;
        using CppClassParent = Gtk::Application_Class;
        using BaseClassParent = GtkApplicationClass;

        friend class Application;

        const Glib::Class& init();
        static void class_init_function(void *g_class, void *class_data);
        static Glib::ObjectBase* wrap_new(GObject*);

    };

    class Application : public Gtk::Application {
    public:
        using CppObjectType = ::Adw::Application;
        using CppClassType = ::Adw::Application_Class;
        using BaseObjectType = AdwApplication;
        using BaseClassType = AdwApplicationClass;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

    private:  friend class Application_Class;
        static CppClassType application_class_;
    protected:
        explicit Application(const Glib::ConstructParams& construct_params);
        explicit Application(AdwApplication* castitem);
    public:
        Application(Application&& src) noexcept;
        Application& operator=(Application&& src) noexcept;
        ~Application() noexcept override;

        /** Get the GType for this class, for use with the underlying GObject type system.
         */
        static GType get_type()      G_GNUC_CONST;
        static GType get_base_type() G_GNUC_CONST;

        ///Provides access to the underlying C GObject.
        AdwApplication*       gobj()       { return reinterpret_cast<AdwApplication*>(gobject_); }

        ///Provides access to the underlying C GObject.
        const AdwApplication* gobj() const { return reinterpret_cast<AdwApplication*>(gobject_); }

        ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
        AdwApplication* gobj_copy();

protected:
        explicit Application(const Glib::ustring& application_id = Glib::ustring(), Gio::Application::Flags flags = Gio::Application::Flags::NONE);

    private:
        const Glib::Class& custom_class_init();

    public:
        static Glib::RefPtr<Application> create(const Glib::ustring& application_id = Glib::ustring(), Gio::Application::Flags flags = Gio::Application::Flags::NONE);

    AdwStyleManager* get_style_manager() { return adw_application_get_style_manager(reinterpret_cast<AdwApplication*>(gobject_)); }
    const AdwStyleManager* get_style_manager() const { return adw_application_get_style_manager(const_cast<AdwApplication*>(reinterpret_cast<const AdwApplication*>(gobject_))); }
    };
}

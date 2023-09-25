#pragma once
#include <adwaita.h>
#include <glibmm.h>
#include <glibmm/class.h>
#include <gtkmm.h>

#include <gtkmm/private/applicationwindow_p.h>

namespace Adw
{

    class ApplicationWindow;
    class ApplicationWindow_Class : public Glib::Class {

    public:
        using CppObjectType = ::Adw::ApplicationWindow;
        using BaseObjectType = AdwApplicationWindow;
        using BaseClassType = AdwApplicationWindowClass;
        using CppClassParent = Gtk::ApplicationWindow_Class;
        using BaseClassParent = GtkApplicationWindowClass;

        friend class ApplicationWindow;

        const Glib::Class& init();
        static void class_init_function(void *g_class, void *class_data);
        static Glib::ObjectBase* wrap_new(GObject*);
    };

    class ApplicationWindow : public Gtk::ApplicationWindow {
    public:
        using CppObjectType = ::Adw::ApplicationWindow;
        using CppClassType = ::Adw::ApplicationWindow_Class;
        using BaseObjectType = AdwApplicationWindow;
        using BaseClassType = AdwApplicationWindowClass;

        ApplicationWindow(ApplicationWindow&& src) noexcept;
        ApplicationWindow& operator=(ApplicationWindow&& src) noexcept;

        ApplicationWindow(const ApplicationWindow&) = delete;
        ApplicationWindow& operator=(const ApplicationWindow&) = delete;

        ~ApplicationWindow() noexcept override;

    private:  friend class ApplicationWindow_Class;
        static CppClassType application_window_class_;

    protected:
        explicit ApplicationWindow(const Glib::ConstructParams& construct_params);
        explicit ApplicationWindow(AdwApplicationWindow* castitem);

    public:
        static GType get_type()      G_GNUC_CONST;
        static GType get_base_type() G_GNUC_CONST;
        AdwApplicationWindow*       gobj()       { return reinterpret_cast<AdwApplicationWindow*>(gobject_); }
        const AdwApplicationWindow* gobj() const { return reinterpret_cast<AdwApplicationWindow*>(gobject_); }

        ApplicationWindow();
        explicit ApplicationWindow(const Glib::RefPtr<Gtk::Application>& application);


        Gtk::Widget* get_content();
        void set_content(Gtk::Widget& content);

    };
}

namespace Glib
{
  /** A Glib::wrap() method for this object.
   *
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Gtk::ApplicationWindow
   */
  GTKMM_API
  Adw::ApplicationWindow* wrap(AdwApplicationWindow* object, bool take_copy = false);
} //namespace Glib

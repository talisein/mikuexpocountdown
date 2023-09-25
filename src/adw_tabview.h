#pragma once
#include <adwaita.h>
#include <glibmm.h>
#include <glibmm/class.h>
#include <gtkmm.h>

#include <gtkmm/private/widget_p.h>

namespace Adw
{

    class TabView;
    class TabView_Class : public Glib::Class {

    public:
        using CppObjectType = ::Adw::TabView;
        using BaseObjectType = AdwTabView;
        using BaseClassType = AdwTabViewClass;
        using CppClassParent = Gtk::Widget_Class;
        using BaseClassParent = GtkWidgetClass;

        friend class TabView;

        const Glib::Class& init();
        static void class_init_function(void *g_class, void *class_data);
        static Glib::ObjectBase* wrap_new(GObject*);
    };

    class TabView : public Gtk::Widget {
    public:
        using CppObjectType = ::Adw::TabView;
        using CppClassType = ::Adw::TabView_Class;
        using BaseObjectType = AdwTabView;
        using BaseClassType = AdwTabViewClass;

        TabView(TabView&& src) noexcept;
        TabView& operator=(TabView&& src) noexcept;

        TabView(const TabView&) = delete;
        TabView& operator=(const TabView&) = delete;

        ~TabView() noexcept override;

    private:  friend class TabView_Class;
        static CppClassType tab_view_class_;

    protected:
        explicit TabView(const Glib::ConstructParams& construct_params);
        explicit TabView(AdwTabView* castitem);

    public:
        static GType get_type()      G_GNUC_CONST;
        static GType get_base_type() G_GNUC_CONST;
        AdwTabView*       gobj()       { return reinterpret_cast<AdwTabView*>(gobject_); }
        const AdwTabView* gobj() const { return reinterpret_cast<AdwTabView*>(gobject_); }

        TabView();

        AdwTabPage* append(Gtk::Widget& child) { return adw_tab_view_append(gobj(), child.gobj()); }
        void close_page(AdwTabPage* page) { adw_tab_view_close_page(gobj(), page); }
        Glib::PropertyProxy< Gtk::Widget* > property_selected_page() { return  Glib::PropertyProxy< Gtk::Widget* >(this, "selected-page"); };
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
   * @relates Gtk::Widget
   */
  GTKMM_API
  Adw::TabView* wrap(AdwTabView* object, bool take_copy = false);
} //namespace Glib

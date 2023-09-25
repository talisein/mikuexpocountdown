#include "adw_tabview.h"

namespace Adw {


    const Glib::Class& TabView_Class::init()
    {
        if(!gtype_) // create the GType if necessary
        {
            // Glib::Class has to know the class init function to clone custom types.
            class_init_func_ = &TabView_Class::class_init_function;

            // This is actually just optimized away, apparently with no harm.
            // Make sure that the parent type has been created.
            //CppClassParent::CppObjectType::get_type();

            // Create the wrapper type, with the same class/instance size as the base type.
            register_derived_type(adw_tab_view_get_type());
        }

        return *this;
    }


    void TabView_Class::class_init_function(void* g_class, void* class_data)
    {
        const auto klass = static_cast<BaseClassType*>(g_class);
        CppClassParent::class_init_function(klass, class_data);


    }

    Glib::ObjectBase* TabView_Class::wrap_new(GObject* o)
    {
        return manage(new TabView((AdwTabView*)(o))); //top-level windows can not be manage()ed.

    }

    TabView::TabView()
        :
        // Mark this class as non-derived to allow C++ vfuncs to be skipped.
        Glib::ObjectBase(nullptr),
        Gtk::Widget(Glib::ConstructParams(tab_view_class_.init()))
    {
    }

    TabView::TabView(const Glib::ConstructParams& construct_params)
        : Gtk::Widget(construct_params)
    {
    }

    TabView::TabView(AdwTabView* castitem)
        : Gtk::Widget((GtkWidget*)(castitem))
    {
    }


    TabView::TabView(TabView&& src) noexcept
        : Gtk::Widget(std::move(src))
    {}

    TabView& TabView::operator=(TabView&& src) noexcept
    {
        Gtk::Widget::operator=(std::move(src));
        return *this;
    }

    TabView::~TabView() noexcept
    {
    }

    TabView::CppClassType TabView::tab_view_class_; // initialize static member

    GType TabView::get_type()
    {
        return tab_view_class_.init().get_type();
    }


    GType TabView::get_base_type()
    {
        return adw_tab_view_get_type();
    }

}

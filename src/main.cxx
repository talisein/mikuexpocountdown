#include "gdkmm/displaymanager.h"
#include "giomm/application.h"
#include "giomm/dbusconnection.h"
#include "giomm/fileicon.h"
#include "glibmm/refptr.h"
#include "glibmm/variant.h"
#include "gtkmm/application.h"
#include "gtkmm/box.h"
#include "gtkmm/enums.h"
#include "gtkmm/icontheme.h"
#include "searchprovider_stub.h"
#include <chrono>
#include <glibmm/objectbase.h>
#include <gtkmm/enums.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/styleprovider.h>
#include <iostream>
#include <iomanip>
#include <gtkmm.h>
#include <pangomm/layout.h>
#include <version>

#if __cpp_lib_chrono < 201907
#include "date/date.h"
#include "date/tz.h"
#endif


namespace {
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono_literals;

    constexpr auto mer_local_time = date::local_days{June/5/2022} + 10h + 30min;
    const auto JST = date::locate_zone("Asia/Tokyo");

    struct raw_event {
        std::string_view name;
        std::string_view style_class;
        date::local_seconds time;
        std::chrono::minutes duration;
    };

    constinit std::array events {std::to_array<raw_event>({
            {"39roove", "roove39", local_days{June/12/2022} + 14h, 6h},
            {"Mirai Tix", "mirai", local_days{June/15/2022} + 12h, 24h * 5 + 12h},
            {"DJ Bar 39", "bar39", local_days{June/15/2022} + 18h, 4h},
            {"Dimension 39", "d39", local_days{July/9/2022} + 23h, 4h},
            {"Mirai Osaka", "mirai", local_days{August/12/2022} + 12h, 48h + 12h},
            {"Mirai Tokyo", "mirai", local_days{September/2/2022} + 12h, 48h + 12h},
            {"Mirai Sapporo", "mirai", local_days{February/4/2023} + 12h, 48h},
            })};
}



class Event : public Glib::Object
{
private:
    Event(const raw_event& event) :
        Glib::ObjectBase("event"),
        property_name(*this, "name", {std::begin(event.name), std::end(event.name)}),
        m_style_class(std::begin(event.style_class), std::end(event.style_class)),
        property_time(*this, "time", {JST, event.time}),
        m_local_time(event.time)
    {
    }

public:
    static Glib::RefPtr<Event> create(const raw_event& event)
    {
        return Glib::RefPtr<Event>(new Event(event));
    }
    virtual ~Event() = default;

    Glib::Property<Glib::ustring> property_name;
    Glib::ustring m_style_class;
    Glib::Property<date::zoned_seconds> property_time;

private:
    date::local_seconds m_local_time;
};

class CountdownGrid : public Gtk::CenterBox
{
public:
    CountdownGrid(const Glib::RefPtr<Event> &event);

    bool update();

    Glib::RefPtr<Event> m_event;
private:
    Gtk::Label *m_days;
    Gtk::Label *m_hours;
    Gtk::Label *m_date;
    Gtk::CenterBox *m_bottom_box;
};

CountdownGrid::CountdownGrid(const Glib::RefPtr<Event> &event) :
    Glib::ObjectBase("CountdownGrid"),
    Gtk::CenterBox(),
    m_event(event),
    m_days(Gtk::manage(new Gtk::Label())),
    m_hours(Gtk::manage(new Gtk::Label())),
    m_date(Gtk::manage(new Gtk::Label())),
    m_bottom_box(Gtk::manage(new Gtk::CenterBox()))
{
    set_orientation(Gtk::Orientation::VERTICAL);
    set_name("page");
    m_days->set_name("days");
    m_hours->set_name("hours");
    m_date->set_name("date");
    set_center_widget(*m_days);
    set_end_widget(*m_bottom_box);
    m_bottom_box->set_valign(Gtk::Align::FILL);
    m_bottom_box->set_vexpand(true);
    m_bottom_box->set_orientation(Gtk::Orientation::VERTICAL);
    m_bottom_box->set_center_widget(*m_hours);
    m_bottom_box->set_end_widget(*m_date);
    m_date->set_valign(Gtk::Align::END);

    auto context = get_style_context();
    context->add_class(event->m_style_class);
    context = get_style_context();
    context->add_class(event->m_style_class);

    auto time = date::make_zoned(date::current_zone(), m_event->property_time.get_value());
    auto timetext = date::format("%c %Z", time);
    auto jsttext = date::format("%c %Z", date::make_zoned(JST, time));
    std::stringstream ss;
    ss << timetext << '\n' << jsttext;
    m_date->set_text(ss.str());
}

bool CountdownGrid::update()
{
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono;

    auto time = m_event->property_time.get_value();
    auto sys_time = time.get_sys_time();
    auto diff_secs = floor<seconds>(sys_time - system_clock::now());
    auto diff_days = floor<std::chrono::days>(diff_secs);

    if (diff_days.count() > 0) {
        std::stringstream ss;
        ss << diff_days.count() << " ";
        if (diff_days.count() > 1) {
            ss << "days";
        } else {
            ss << "day";
        }
        m_days->set_label(ss.str());
    } else {
        if (diff_secs.count() > 0) {
            m_days->set_wrap(true);
            m_days->set_wrap_mode(Pango::WrapMode::WORD);
            m_days->set_label("Soon");
        } else {
            m_days->set_label("Miku time now!");
            m_days->set_wrap(true);
            return false;
        }
    }

    std::stringstream ss;
    ss << make_time(diff_secs - diff_days);
    m_hours->set_label(ss.str());

    return true;
}

class CountdownWindow : public Gtk::ApplicationWindow
{
public:
    CountdownWindow() :
        Glib::ObjectBase("CountdownWindow"),
        m_box(Gtk::manage(new Gtk::Box(Gtk::Orientation::VERTICAL))),
        m_stack(Gtk::manage(new Gtk::Stack())),
        m_stack_switcher(Gtk::manage(new Gtk::StackSwitcher()))
    {
        set_name("mainwin");
        m_box->append(*m_stack_switcher);
        m_box->append(*m_stack);
        m_stack_switcher->set_stack(*m_stack);
        m_stack_switcher->set_halign(Gtk::Align::CENTER);

        auto provider = Gtk::CssProvider::create();
        provider->load_from_resource("/org/talinet/mikuexpocountdown/style.css");
        auto style_context = get_style_context();
        for (auto &display : Gdk::DisplayManager::get()->list_displays()) {
            style_context->add_provider_for_display(display, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }

        set_child(*m_box);
        m_stack->set_expand(true);
        m_stack->property_visible_child().signal_changed().connect(sigc::mem_fun(*this, &CountdownWindow::on_switch));

        for (auto &e : events) {
            auto event = Event::create(e);
            auto grid = Gtk::manage(new CountdownGrid(event));
            m_stack->add(*grid, event->property_name.get_value(), event->property_name.get_value());
            grid->update();
            Glib::signal_timeout().connect_seconds(sigc::mem_fun(*grid, &CountdownGrid::update), 1);
        }
    }

    virtual ~CountdownWindow() = default;

private:
    Gtk::Box *m_box;
    Gtk::Stack *m_stack;
    Gtk::StackSwitcher *m_stack_switcher;

    void on_switch() {
        auto event = static_cast<CountdownGrid*>(m_stack->property_visible_child().get_value())->m_event;
        auto style_class = event->m_style_class;
        auto context = m_box->get_style_context();
        for (auto &e : events) {
            Glib::ustring style_class{e.style_class.begin(), e.style_class.end()};
            if (context->has_class(style_class)) context->remove_class(style_class);
        }
        context->add_class(style_class);
        set_title(Glib::ustring::compose("%1 Countdown", event->property_name.get_value()));
    }
};

class SearchProvider : public org::gnome::Shell::SearchProvider2Stub
{
    virtual void
    GetInitialResultSet(const std::vector<Glib::ustring> & terms,
                        MethodInvocation &invocation) override final
    {
        const auto miku = Glib::ustring("miku").casefold();
        std::vector<Glib::ustring> results;
        for (auto &e : events) {
            const auto event = Glib::ustring(e.name.begin(), e.name.end()).casefold();
            const auto theme = Glib::ustring(e.style_class.begin(), e.style_class.end()).casefold();
            for (const auto &term : terms) {
                auto folded = term.casefold();
                if (Glib::ustring::npos != event.find(folded) ||
                    Glib::ustring::npos != theme.find(folded) ||
                    Glib::ustring::npos != miku.find(folded)) {
                    results.emplace_back(event);
                    break;
                }
            }
        }

        invocation.ret(results);
    }

    virtual void
    GetSubsearchResultSet(const std::vector<Glib::ustring> & previous_results,
                          const std::vector<Glib::ustring> & terms,
                          MethodInvocation &invocation) override final
    {
        const auto miku = Glib::ustring("miku").casefold();
        std::vector<Glib::ustring> results;
        for (auto &e : events) {
            const auto event = Glib::ustring(e.name.begin(), e.name.end()).casefold();
            const auto theme = Glib::ustring(e.style_class.begin(), e.style_class.end()).casefold();
            for (const auto &term : terms) {
                auto folded = term.casefold();
                if (Glib::ustring::npos != event.find(folded) ||
                    Glib::ustring::npos != theme.find(folded) ||
                    Glib::ustring::npos != miku.find(folded)) {
                    results.emplace_back(event);
                    break;
                }
            }
        }
        invocation.ret(results);
    }
    virtual void
    GetResultMetas(const std::vector<Glib::ustring> & identifiers,
                   MethodInvocation &invocation) override final
    {
        std::vector<std::map<Glib::ustring,Glib::VariantBase>> v;
        for (auto &id : identifiers) {
            auto it = std::find_if(std::begin(events), std::end(events), [&id](const raw_event& event) {
                return Glib::ustring(event.name.begin(), event.name.end()).casefold().compare(id) == 0;
            });
            if (std::end(events) != it) {
                std::map<Glib::ustring, Glib::VariantBase> m;
                m.insert({"id", Glib::Variant<Glib::ustring>::create(id)});
                m.insert({"name", Glib::Variant<Glib::ustring>::create(Glib::ustring(std::begin(it->name), std::end(it->name)))});
                m.insert({"gicon", Glib::Variant<Glib::ustring>::create("dance._39music.MikuExpoCountdown")});
                auto jst_time = date::make_zoned(JST, it->time);
                m.insert({"description", Glib::Variant<Glib::ustring>::create(date::format("%c %Z", date::make_zoned(date::current_zone(), jst_time)))});
                v.push_back(m);
            }
        }
        invocation.ret(v);
    }

    virtual void
    ActivateResult(const Glib::ustring & identifier,
                   const std::vector<Glib::ustring> & terms,
                   guint32 timestamp,
                   MethodInvocation &invocation) override final
    {
        invocation.ret();
    }

    virtual void
    LaunchSearch(const std::vector<Glib::ustring> & terms,
                 guint32 timestamp,
                 MethodInvocation &invocation) override final
    {
        invocation.ret();
    }
};

class MikuApplication : public Gtk::Application
{
public:
    MikuApplication(const Glib::ustring &application_id) : Glib::ObjectBase("MikuApplication"), Gtk::Application(application_id) {
    }

    virtual ~MikuApplication() = default;

    template<typename... Args>
    static Glib::RefPtr<MikuApplication> create(Args... args) { return Glib::make_refptr_for_instance<MikuApplication>(new MikuApplication(args...)); }

    protected:

    virtual bool
    dbus_register_vfunc (const Glib::RefPtr<Gio::DBus::Connection> &connection, const Glib::ustring &object_path) override final
    {
        auto res = Gtk::Application::dbus_register_vfunc(connection, object_path);
        search.register_object(connection, "/dance/_39music/MikuExpoCountdown/SearchProvider");
        return res;
    }

    virtual void
    dbus_unregister_vfunc (const Glib::RefPtr<Gio::DBus::Connection> &connection, const Glib::ustring &object_path) override final
    {
        search.unregister_object();
        Gtk::Application::dbus_unregister_vfunc(connection, object_path);
    }

private:
    SearchProvider search;
};

int main (int argc, char *argv[])
{
    auto app = MikuApplication::create("dance._39music.MikuExpoCountdown");
    return app->make_window_and_run<CountdownWindow>(argc, argv);
}

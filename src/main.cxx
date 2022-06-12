#include <version>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <gtkmm.h>
#include "searchprovider_stub.h"

#if __cpp_lib_chrono < 201907
#include "date/date.h"
#include "date/tz.h"
#endif


namespace {
#if __cpp_lib_chrono < 201907
    using namespace date;
#endif
    using namespace std::chrono_literals;

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
        m_duration(event.duration),
        m_local_time(event.time)
    {
    }

public:
    static Glib::RefPtr<Event> create(const raw_event& event)
    {
        return Glib::RefPtr<Event>(new Event(event));
    }
    virtual ~Event() = default;

    bool is_live() const { return 0s > secs_to_live(); }

    bool is_expired() const { return 0s > secs_to_expire(); }

    std::chrono::seconds secs_to_live() const {
        return floor<std::chrono::seconds>(property_time.get_value().get_sys_time()
                                           - std::chrono::system_clock::now());
    }
    std::chrono::seconds secs_to_expire() const {
        return secs_to_live() + m_duration;
    }

    Glib::Property<Glib::ustring> property_name;
    Glib::ustring m_style_class;
    Glib::Property<date::zoned_seconds> property_time;
    std::chrono::minutes m_duration;
private:
    date::local_seconds m_local_time;
};

class CountdownGrid : public Gtk::CenterBox
{
public:
    CountdownGrid(const Glib::RefPtr<Event> &event);

    bool update();
    bool remove_me() {
        auto stack = dynamic_cast<Gtk::Stack*>(this->get_parent());
        if (stack) {
            auto visible_child = stack->get_visible_child();
            stack->remove(*this);
            if (static_cast<Gtk::Widget*>(this) == visible_child) {
                stack->set_visible_child(*(stack->get_first_child()));
            }
        }
        return false;
    }
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

    m_days->set_max_width_chars(8); // '999 days'
    m_days->set_wrap(true);
    m_days->set_justify(Gtk::Justification::CENTER);
    auto attrs = Pango::AttrList();
    auto line_height = Pango::Attribute::create_attr_line_height(0.6);
    attrs.insert(line_height);
    m_days->set_attributes(attrs);

    auto context = get_style_context();
    context->add_class(event->m_style_class);
    context = get_style_context();
    context->add_class(event->m_style_class);

    auto localtime = date::make_zoned(date::current_zone(), m_event->property_time.get_value());
    auto jsttime   = date::make_zoned(JST, localtime);
    m_date->set_text(Glib::ustring::compose("%1\n%2",
                                            date::format("%c %Z", localtime),
                                            date::format("%c %Z", jsttime)));
    update();
}

bool CountdownGrid::update()
{
    auto diff_secs = m_event->secs_to_live();
    auto diff_days = floor<std::chrono::days>(diff_secs);

    if (diff_days > 0s) {
        m_hours->show();
        if (diff_days.count() > 1) {
            m_days->set_label(Glib::ustring::compose("%1 days", diff_days.count()));
        } else {
            m_days->set_label(Glib::ustring::compose("%1 day", diff_days.count()));
        }
        m_hours->set_label(date::format("%T", diff_secs - diff_days));
    } else {
        if (diff_secs > 0s) {
            m_days->set_label("Soon");
            m_hours->set_label(date::format("%T", diff_secs));
        } else {
            m_hours->hide();
            m_days->set_label("Miku time now!");
            auto secs_to_end = m_event->secs_to_expire();
            if (secs_to_end > 0s) {
                Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &CountdownGrid::remove_me), secs_to_end.count());
            } else {
                remove_me();
            }
            return false;
        }
    }

    return true;
}

class CountdownWindow : public Gtk::ApplicationWindow
{
public:
    CountdownWindow(const Glib::RefPtr<Gtk::Application>& app) :
        Glib::ObjectBase("CountdownWindow"),
        Gtk::ApplicationWindow(app),
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
            if (event->is_expired()) continue;
            auto grid = Gtk::manage(new CountdownGrid(event));
            m_stack->add(*grid, event->property_name.get_value(), event->property_name.get_value());
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
                    results.push_back(std::move(event));
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

                auto sys_time = jst_time.get_sys_time();
                auto diff_secs = floor<std::chrono::seconds>(sys_time - std::chrono::system_clock::now());
                auto diff_days = floor<std::chrono::days>(diff_secs);

                std::stringstream ss;
                if (diff_days.count() > 0) {
                    ss << "in about " << diff_days.count() << " day";
                    if (diff_days.count() > 1)
                        ss << 's';
                } else {
                    auto diff_hours = floor<std::chrono::hours>(diff_secs);
                    if (diff_hours.count() > 0) {
                        ss << "in about " << diff_hours.count() << " hour";
                        if (diff_hours.count() > 1)
                            ss << 's';
                    } else {
                        auto diff_minutes = floor<std::chrono::minutes>(diff_secs);
                        if (diff_minutes.count() < 1) {
                            if (diff_secs.count() <= 0) {
                                if ((diff_secs + it->duration).count() > 0) {
                                    ss << "Happening now";
                                } else {
                                    ss << "Event finished";
                                }
                            } else {
                                ss << "in less than a minute!";
                            }
                        } else if (diff_minutes.count() == 1) {
                            ss << "in a minute!";
                        } else {
                            ss << "in about " << diff_minutes.count() << " minutes";
                        }
                    }
                }
                Glib::ustring display_date;
                if (diff_secs.count() <= 0) { // Event started
                    auto end_time = date::make_zoned(JST, it->time + it->duration);
                    if ((diff_secs + it->duration).count() > 0) // Event ongoing
                        display_date = Glib::ustring::compose("until %1", date::format("%c %Z", date::make_zoned(date::current_zone(), end_time)));
                    else // Event finished
                        display_date = Glib::ustring::compose("ended %1", date::format("%c %Z", date::make_zoned(date::current_zone(), end_time)));
                } else { // Event starts in the future
                    display_date = date::format("%c %Z", date::make_zoned(date::current_zone(), jst_time));
                }
                auto description = Glib::ustring::compose("%1 \u2014 %2",
                                                          ss.str(),
                                                          display_date);
                m.insert({"description", Glib::Variant<Glib::ustring>::create(description)});
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

    if constexpr (!GTKMM_CHECK_VERSION(4,8,0)) {
        Gtk::Window *m_run_window = nullptr;
        app->signal_activate().connect([&app, &m_run_window] () {
            if (!m_run_window) {
                auto window = new CountdownWindow(app);
                bool was_floating = g_object_is_floating(window->gobj());
                window->Gtk::Object::set_manage();
                if (!was_floating && g_object_is_floating(window->gobj()))
                    g_object_ref_sink(window->gobj());
                m_run_window = window;
                app->add_window(*window);
                window->set_application(app);
                window->present();
            } else {
                m_run_window->present();
            }
        });

        app->signal_window_removed().connect([&m_run_window] (Gtk::Window* window) {
            if (window == m_run_window)
            {
                if (window && window->gobj())
                    gtk_window_destroy(window->gobj());
                m_run_window = nullptr;
            }
        });

        app->run(argc, argv);
    } else {
        return app->make_window_and_run<CountdownWindow>(argc, argv, app);
    }
}

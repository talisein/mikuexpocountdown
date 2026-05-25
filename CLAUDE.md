# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A GTK4/Adwaita desktop app (GNOME) that counts down to upcoming Hatsune Miku Expo and Magical Mirai concerts. It also registers as a GNOME Shell search provider so events are searchable from the Activities overlay.

## Build commands

```sh
# Configure (once, or after meson.build changes)
meson setup build

# Build
ninja -C build

# Run the app (after build, without installing)
build/src/mikuexpo-countdown

# Install (needed to register the GSettings schema, D-Bus service, and search provider)
sudo ninja -C build install

# Run all tests
meson test -C build

# Run a single test
meson test -C build variant-utils
```

The build directory is `build/`. The installed binary is `mikuexpo-countdown`.

Tests are defined in `src/meson.build` (C++ unit test) and `res/meson.build` (schema validation, desktop file validation, metainfo validation).

## Build option: bundled IANA tzdata

There is a GCC 14 stdlib bug (bug 116657) that causes `std::chrono::locate_zone` to fail in the Flatpak environment. The workaround is:

```sh
meson setup build -Dbundle_iana_tzdata=true
```

This installs `res/tzdata-2024a.zi` alongside the binary and activates a `__gnu_cxx::zoneinfo_dir_override()` hook in `events.cpp` to redirect zone lookups there. Default is `false`.

## Architecture

### GObject bindings: peel

This project uses **peel** (`subprojects/peel/`) instead of gtkmm. Peel is a zero-overhead, header-only C++ binding for GLib/GTK/Adwaita generated from GObject introspection data. Includes include paths like `<peel/Gtk/Box.h>`, `<peel/Adw/Application.h>`, etc. Generated bindings are produced at build time by `peel-gen` into `build/peel/`.

All custom GObject subclasses use the `PEEL_SIMPLE_CLASS` macro and `PEEL_CLASS_IMPL` macro. The class pattern is:
- Declare with `PEEL_SIMPLE_CLASS(ClassName, ParentClass)` in the header
- Implement with `PEEL_CLASS_IMPL(ClassName, "GTypeName", ParentClass)` in the `.cpp`
- Override virtual functions by name: `override_vfunc_dispose<T>()`, `parent_vfunc_dispose<T>()`, etc.
- Member data is kept in an embedded `struct Members { ... } m;` rather than as individual fields

Smart pointer types: `peel::RefPtr<T>` (ref-counted), `peel::UniquePtr<T>` (owned), `peel::SignalConnection` for signal lifetimes.

#### GObject properties

Declare each property in the class body with `PEEL_PROPERTY(type, c_name, "gobject-name")` — this generates a static `prop_c_name()` method returning `peel::Property<type>`. Supported types map directly to GObject param specs: `const char*` → `G_TYPE_STRING`, `int64_t` → `G_TYPE_INT64`, `bool`, `int`, `unsigned`, `double`, GObject-derived pointers, etc. (`peel::String` is interchangeable with `const char*`.)

Wire up the read/write logic with a private `define_properties` static template — peel detects it automatically and installs the param specs and `get_property`/`set_property` vfuncs at class-init time, so `Class::init()` needs no changes:

```cpp
template<typename F>
static void define_properties(F &f) {
    f.prop(prop_name(), (const char*)nullptr).get(&MyClass::name);
    f.prop(prop_count(), 0, INT_MAX, 0).get(&MyClass::get_count).set(&MyClass::set_count);
}
```

The getter method can return any type implicitly convertible to the property type (e.g. `const peel::String&` works as a getter for a `const char*` property via `operator const char*()`). Emit `notify(prop_foo())` after mutating a property so bindings and watchers update.

Read a property from outside: `obj->get_property(MyClass::prop_name())` returns the `OwnedType` for that property type (`peel::String` for `const char*`, `peel::RefPtr<T>` for GObject types, the value type itself for scalars).

#### Property bindings

```cpp
#include <peel/GObject/BindingFlags.h>

peel::GObject::Object::bind_property(
    source, SourceClass::prop_foo(),
    target, TargetClass::prop_bar(),
    peel::GObject::BindingFlags::SYNC_CREATE);  // or DEFAULT, BIDIRECTIONAL, etc.
```

`SYNC_CREATE` fires immediately to sync the target, then keeps it in sync. The binding lifetime is managed automatically — it is invalidated when either source or target is finalized, so it rarely needs to be stored explicitly.

A transform lambda can be supplied as a fifth argument. Peel detects the right overload: if the lambda takes the source property's `UnownedType` and returns something assignable to the target property, it is called with the unpacked value:

```cpp
peel::GObject::Object::bind_property(
    tab_view, peel::Adw::TabView::prop_selected_page(),
    window_title, peel::Adw::WindowTitle::prop_title(),
    peel::GObject::BindingFlags::SYNC_CREATE,
    [](peel::Adw::TabPage *page) -> peel::String {
        if (!page) return {};
        return format_to_peel("{} Countdown", page->get_child()->cast<MyWidget>()->name());
    });
```

### Event data (events.cpp)

All concert events are hardcoded as `raw_event` structs in a `std::array` in `events.cpp`. Each entry has a name, search terms, style class, `std::chrono::local_seconds` time, duration, and `std::chrono::time_zone*`. Timezone pointers are resolved at startup into a static `tz_class` struct.

`Miku::get_events()` returns a view over a static `std::vector<peel::RefPtr<Miku::Event>>` built once at first call. It also dynamically appends Miku's birthday (August 31) for the current or next year.

**To add a new event:** add an entry to the `raw_events` array in `events.cpp`, add any new timezone pointer to `tz_class` if needed, and add matching background/hero images to `res/` and reference them in `res/style.css` using the style class name.

### Widget hierarchy

- `MikuApplication` (`application.cpp`) — subclasses `peel::Adw::Application`; creates `CountdownWindow` on activate, registers `SearchProvider` over D-Bus
- `CountdownWindow` (`window.cpp`) — subclasses `peel::Adw::ApplicationWindow`; holds an `Adw::TabView` with one tab per unexpired event; persists selected tab to GSettings
- `CountdownGrid` (`grid.cpp`) — one per event tab; subclasses `peel::Gtk::Box`; drives a per-second GLib timeout to update labels; emits `signal_expired` when its event passes, causing the window to remove the tab
- `SearchProvider` (`search_provider.cpp`) — subclasses `peel::Gio::DBusInterfaceSkeleton`; implements the GNOME Shell SearchProvider2 D-Bus interface; emits `s_activate(name)` when the user clicks a search result, which routes to `CountdownWindow::activate_page()`

### Resources

`res/mikuexpocountdown.gresource.xml` bundles all images and `style.css` into the binary as a GResource. Each event tour has a `style_class` string (e.g., `"expoasia2025"`, `"expona2026"`, `"expoeu2026"`, `"mm2026"`) that maps to CSS rules in `res/style.css` for per-tour background and hero images.

### Utility headers

- `util.hpp` — string helpers between `peel::String` and `std::string_view`; `format_to_peel()` for writing `std::format` output directly into a GLib-allocated `peel::String`
- `variant_utils.hpp` — helpers to extract strings from `peel::GLib::Variant*`

### Flatpak

`dance._39music.MikuExpoCountdown.yaml` builds the app as a Flatpak against `org.gnome.Platform`. The `peel` subproject is sourced from its upstream git repo. Built with `--wrap-mode=nodownload` to prevent Meson from fetching anything else at build time.

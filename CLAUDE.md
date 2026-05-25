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

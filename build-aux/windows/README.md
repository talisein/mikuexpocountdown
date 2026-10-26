# Windows packaging

This directory builds a per-user `.msi` for Miku Expo Countdown, using the
same AsusMiku setup and the same layout as Monday Night Net
(`~/git/mnn/build-aux/windows/README.md`). That README explains the stage,
the relocation rules and the MSIX constraints in more detail. This one covers
only what is different here.

The real product is the **staged folder**, `build-msvc\stage\`: a
self-contained, relocatable `bin\ lib\ share\` tree. The `.msi` is a thin
WiX wrapper around it.

| File | What it does |
|---|---|
| `stage.ps1` | Builds `build-msvc\stage\` from scratch and self-checks it |
| `make-msi.ps1` | Runs `stage.ps1`, then `wix build` and ICE validation |
| `mikuexpocountdown.wxs` | The WiX package: harvests the stage, adds the Start menu shortcut |
| `mikuexpocountdown-test.wsb` | A Windows Sandbox config with `build-msvc` mapped read-only |

## Prerequisites (on the build box)

Run these from an **unelevated** PowerShell on the desktop, not over ssh.
They are in `~/AsusMiku` and are idempotent.

1. `setup-gtk-deps.ps1`: the MSVC GTK 4 stack in `C:\gtk-build`. **This app
   also needs the WebP pixbuf loader that it builds** (libwebp and
   webp-pixbuf-loader). Most of the backgrounds are `.webp`, and GTK decodes
   only PNG, JPEG and TIFF itself.
2. `setup-installer-tools.ps1`: the .NET SDK and WiX.

The build dir must use meson's default prefix (`C:\`), which DESTDIR turns
into the stage root. peel comes from `subprojects/peel.wrap`, pinned to the
same commit as the flatpak manifest.

    C:\dev\gtkenv.cmd cd /d C:\dev\mikuexpocountdown && meson setup --native-file C:\dev\clang-cl-c.ini build-msvc

## Building

    C:\dev\gtkenv.cmd cd /d C:\dev\mikuexpocountdown && powershell -ExecutionPolicy Bypass -File build-aux\windows\make-msi.ps1

This writes `build-msvc\mikuexpo-countdown-<version>-x64.msi`. The version
comes from `meson.build` and must be numeric `x.y.z`.

## Testing

Run `start build-aux\windows\mikuexpocountdown-test.wsb`. The sandbox has no
network, because the app needs none. Inside it:

1. Run `C:\miku\stage\bin\mikuexpo-countdown.exe` straight from the
   read-only mapping.
2. Install `C:\miku\mikuexpo-countdown-<version>-x64.msi`. There should be
   no UAC prompt. Launch it from the Start menu, then check:
   - the backgrounds and character art show (WebP and PNG/JPEG);
   - the countdown shows the right local time for an event;
   - the selected tab is remembered after a restart (GSettings, in the
     registry under `HKCU\Software\GSettings`).
3. Uninstall it from Settings → Apps.

## What is different from mnn

- **Plugins:** the SVG and **WebP** gdk-pixbuf loaders. No gio TLS module,
  because the app makes no network connections.
- **No translations:** the app has no catalogs, so GTK's would never be
  read (see mnn's README).
- **Linux-only files are not installed on Windows:** the desktop file,
  metainfo, D-Bus service, systemd unit and GNOME Shell search provider.
  Without `gdbus.exe`, GApplication has no session bus, so the search
  provider is never exported.
- **Time zones:** MSVC's `std::chrono::locate_zone` uses Windows' ICU, so
  no tzdata is shipped. Leave `bundle_iana_tzdata` off.
- **Fonts:** `style.css` asks for Nanum Pen Script and Free Mono, which
  Windows lacks. GTK falls back to the system fonts.

## Identity

| | |
|---|---|
| Product name | Miku Expo Countdown |
| Publisher display name | talisein |
| AppUserModelID | `dance._39music.MikuExpoCountdown` |
| MSI UpgradeCode | `D5181285-5E04-4AC3-A1B2-68B566D5765C` (never change it) |
| Install folder | `%LOCALAPPDATA%\Programs\Miku Expo Countdown` |
| Shortcut key path | `HKCU\Software\talisein\Miku Expo Countdown` |

# mikuexpo-countdown

![screenshot](screenshots/screenshot.png)

How long until Miku Expo? How long until Magical Mirai? The answer to these
questions will be at your fingertips with this toy app.

If you'd like a Flatpak, you can click this button:

<a href='https://flatpak.39music.dance/repo/stable/appstream/dance._39music.MikuExpoCountdown.flatpakref'><img height='51' alt='Download on 39music' src='https://flatpak.39music.dance/repo/stable/appstream/39music-badge.png'/></a>


Otherwise, to build it yourself you'll need gtkmm-4 and a C++23 compiler.

Fedora 40 instructions:
```
# sudo dnf install gtkmm4.0-devel meson gcc-c++ libadwaita-devel
# meson setup build
# ninja -C build
# # Install to e.g. /usr/local. Needed to install gsettings schema.
# sudo ninja -C build install
# build/src/mikuexpo-countdown # (This runs the program)
```

Other distros you can figure out yourself..!

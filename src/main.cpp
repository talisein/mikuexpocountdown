#include <chrono>
#include <memory>
#include <glib.h>
#include "application.hpp"

#ifdef G_OS_WIN32
#include <windows.h>
#include <appmodel.h>
#include <shobjidl.h>
#include <peel/GLib/functions.h>
#endif

namespace {
#ifdef G_OS_WIN32
    /* Installed as <root>\bin\mikuexpo-countdown.exe. GLib and GTK find their
     * own data relative to their DLLs; see build-aux\windows\README.md. */
    void relocate_win32() noexcept
    {
        auto root = peel::String::adopt_string(g_win32_get_package_installation_directory_of_module(nullptr));

        /* librsvg's libxml2 reads its default catalog from the absolute
         * prefix it was built for, which on another machine anyone may
         * create. Point it into the install root, where no catalog exists. */
        auto xml_catalog = peel::GLib::build_filename(root, "etc", "xml", "catalog");
        peel::GLib::setenv("XML_CATALOG_FILES", xml_catalog, false);

        /* An MSIX package gets its AUMID from the OS; only an unpackaged
         * install sets it, to match its Start menu shortcut. */
        UINT32 len = 0;
        if (GetCurrentPackageFullName(&len, nullptr) == APPMODEL_ERROR_NO_PACKAGE) {
            std::unique_ptr<gunichar2, decltype(&g_free)> aumid {
                g_utf8_to_utf16("dance._39music.MikuExpoCountdown", -1, nullptr, nullptr, nullptr), &g_free };
            SetCurrentProcessExplicitAppUserModelID(reinterpret_cast<PCWSTR>(aumid.get()));
        }
    }
#endif
}

int main(int argc, char *argv[]) {
#ifdef G_OS_WIN32
    relocate_win32();
#endif
    auto app = MikuApplication::create();
    return app->run(argc, argv);
}

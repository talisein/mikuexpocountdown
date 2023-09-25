#include <version>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <gtkmm.h>

#if __cpp_lib_chrono < 201907
#include "date/date.h"
#include "date/tz.h"
#endif

#include "application.h"
#include "window.h"

int main (int argc, char *argv[])
{
    auto app = MikuApplication::create("dance._39music.MikuExpoCountdown");

    app->make_window_and_run<CountdownWindow>(argc, argv, app);
}

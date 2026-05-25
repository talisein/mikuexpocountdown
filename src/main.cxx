#include <version>
#include <chrono>
#if __cpp_lib_chrono < 201907
#include "date/date.h"
#include "date/tz.h"
#endif
#include "application.h"

int main(int argc, char *argv[]) {
    auto app = MikuApplication::create();
    return app->run(argc, argv);
}

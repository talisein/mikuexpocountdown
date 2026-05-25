#include <chrono>
#include "application.hpp"

int main(int argc, char *argv[]) {
    auto app = MikuApplication::create();
    return app->run(argc, argv);
}

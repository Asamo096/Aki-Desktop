#include "aki/server.h"
#include "aki/utils/log.h"

int main(int argc, char *argv[]) {
    aki::utils::Log::init();
    aki::utils::Log::info("Starting Aki Desktop...");

    aki::Server server;
    if (!server.init()) {
        aki::utils::Log::fatal("Failed to initialize server");
        return 1;
    }

    server.run();
    return 0;
}

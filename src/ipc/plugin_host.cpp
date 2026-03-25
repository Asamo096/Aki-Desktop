#include "aki/ipc/plugin_host.h"
#include "aki/server.h"
#include "aki/utils/log.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace aki {

namespace ipc {

PluginHost::PluginHost(Server *server) :
    server(server) {
}

PluginHost::~PluginHost() {
    stop_all_plugins();
}

bool PluginHost::init() {
    utils::Log::info("Plugin host initialized");
    return true;
}

bool PluginHost::start_plugin(const std::string &path) {
    pid_t pid = fork();
    if (pid < 0) {
        utils::Log::error("Failed to fork plugin process");
        return false;
    } else if (pid == 0) {
        // 子进程
        execl(path.c_str(), path.c_str(), nullptr);
        utils::Log::error("Failed to execute plugin");
        exit(1);
    } else {
        // 父进程
        plugin_pids.push_back(pid);
        utils::Log::info("Plugin started with PID: " + std::to_string(pid));
        return true;
    }
}

void PluginHost::stop_all_plugins() {
    for (pid_t pid : plugin_pids) {
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
    }
    plugin_pids.clear();
    utils::Log::info("All plugins stopped");
}

} // namespace ipc
} // namespace aki

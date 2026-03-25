#pragma once

#include <string>
#include <vector>

namespace aki {

class Server;

namespace ipc {

class PluginHost {
private:
    Server *server;
    std::vector<pid_t> plugin_pids;

public:
    PluginHost(Server *server);
    ~PluginHost();

    bool init();
    bool start_plugin(const std::string &path);
    void stop_all_plugins();
};

} // namespace ipc
} // namespace aki

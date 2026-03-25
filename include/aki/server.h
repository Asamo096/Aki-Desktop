#pragma once

#include <wayland-server.h>
#include <wlroots.h>
#include <vector>

namespace aki {

class Output;
class Input;
class View;

namespace config {
    class Config;
    class LuaAPI;
}

namespace ipc {
    class PluginHost;
}

using namespace config;
using namespace ipc;

class Server {
private:
    wl_display *display;
    wl_event_loop *event_loop;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_compositor *compositor;
    struct wlr_output_layout *output_layout;
    struct wlr_xdg_shell *xdg_shell;
    Config *config;
    LuaAPI *lua_api;
    PluginHost *plugin_host;
    std::vector<Output*> outputs;
    std::vector<Input*> inputs;
    std::vector<View*> views;

public:
    Server();
    ~Server();

    bool init();
    void run();
    void shutdown();

    wl_display *get_display() const;
    struct wlr_backend *get_backend() const;
    struct wlr_renderer *get_renderer() const;
    struct wlr_output_layout *get_output_layout() const;

    void add_output(Output *output);
    void remove_output(Output *output);
    
    void add_input(Input *input);
    void remove_input(Input *input);
    
    void add_view(View *view);
    void remove_view(View *view);
    const std::vector<View*>& get_views() const;
    
    // 输入延迟测量
    uint64_t get_last_input_time() const;
    
    // 配置系统
    bool init_config();
    bool load_config(const std::string &path);
    bool reload_config();
    Config *get_config() const;
    LuaAPI *get_lua_api() const;
    
    // 插件系统
    bool init_plugins();
    bool start_plugin(const std::string &path);
    void stop_all_plugins();
    PluginHost *get_plugin_host() const;
};

} // namespace aki

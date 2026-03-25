#include "aki/server.h"
#include "aki/output.h"
#include "aki/input.h"
#include "aki/view.h"
#include "aki/config/config.h"
#include "aki/config/lua_api.h"
#include "aki/ipc/plugin_host.h"
#include "aki/render/renderer.h"
#include "aki/utils/log.h"

#include <wayland-server.h>
#include <wlroots.h>
#include <signal.h>
#include <execinfo.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

namespace aki {

// 静态指针，用于信号处理
static Server *g_server_instance = nullptr;

// 信号处理函数
static void signal_handler(int signum) {
    if (g_server_instance) {
        utils::Log::info("Received signal " + std::to_string(signum) + ", shutting down...");
        g_server_instance->shutdown();
        exit(0);
    }
}

// 崩溃转储函数
static void crash_handler(int signum) {
    utils::Log::error("Received crash signal " + std::to_string(signum) + ", generating crash dump...");
    
    // 收集堆栈信息
    void *backtrace_buffer[64];
    int backtrace_size = backtrace(backtrace_buffer, 64);
    
    // 生成崩溃转储文件
    std::ofstream crash_dump("aki_crash.log");
    crash_dump << "Crash signal: " << signum << "\n";
    crash_dump << "Stack trace:\n";
    
    char **backtrace_symbols = backtrace_symbols(backtrace_buffer, backtrace_size);
    if (backtrace_symbols) {
        for (int i = 0; i < backtrace_size; i++) {
            crash_dump << backtrace_symbols[i] << "\n";
        }
        free(backtrace_symbols);
    }
    
    crash_dump.close();
    
    // 退出程序
    if (g_server_instance) {
        g_server_instance->shutdown();
    }
    exit(1);
}

Server::Server() :
    display(nullptr),
    event_loop(nullptr),
    backend(nullptr),
    renderer(nullptr),
    compositor(nullptr),
    output_layout(nullptr),
    xdg_shell(nullptr),
    config(nullptr),
    lua_api(nullptr),
    plugin_host(nullptr) {
    // 设置全局实例指针
    g_server_instance = this;
}

Server::~Server() {
    shutdown();
}

bool Server::init() {
    // 设置信号处理
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    // 设置崩溃信号处理
    signal(SIGSEGV, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGILL, crash_handler);
    signal(SIGFPE, crash_handler);
    
    // 创建 Wayland 显示
    display = wl_display_create();
    if (!display) {
        utils::Log::error("Failed to create Wayland display");
        return false;
    }

    event_loop = wl_display_get_event_loop(display);

    // 创建后端
    backend = wlr_backend_autocreate(display);
    if (!backend) {
        utils::Log::error("Failed to create backend");
        return false;
    }

    // 创建渲染器
    renderer = wlr_renderer_autocreate(backend);
    if (!renderer) {
        utils::Log::error("Failed to create renderer");
        return false;
    }

    // 创建合成器
    compositor = wlr_compositor_create(display, renderer);
    if (!compositor) {
        utils::Log::error("Failed to create compositor");
        return false;
    }

    // 创建xdg-shell
    xdg_shell = wlr_xdg_shell_create(display);
    if (!xdg_shell) {
        utils::Log::error("Failed to create xdg shell");
        return false;
    }

    // xdg-shell 新表面事件处理
    auto xdg_surface_handler = [](void *data, struct wl_listener *listener, void *xdg_surface) {
        Server *server = static_cast<Server*>(data);
        struct wlr_xdg_surface *wlr_xdg_surface = static_cast<struct wlr_xdg_surface*>(xdg_surface);
        
        // 创建View对象
        View *view = new View(server, wlr_xdg_surface->surface);
        view->xdg_surface = wlr_xdg_surface;
        
        // 添加到服务器
        server->add_view(view);
        
        utils::Log::info("New xdg surface created");
    };

    struct wl_listener *xdg_surface_add = wl_listener_create();
    xdg_surface_add->notify = xdg_surface_handler;
    xdg_surface_add->data = this;
    wl_signal_add(&xdg_shell->events.new_surface, xdg_surface_add);

    // 创建输出布局
    output_layout = wlr_output_layout_create();
    if (!output_layout) {
        utils::Log::error("Failed to create output layout");
        return false;
    }

    // 输出管理
    auto output_handler = [](void *data, struct wl_listener *listener, void *output) {
        Server *server = static_cast<Server*>(data);
        auto *wlr_output = static_cast<struct wlr_output*>(output);
        Output *new_output = new Output(server, wlr_output);
        server->add_output(new_output);
    };

    struct wl_listener *output_add = wl_listener_create();
    output_add->notify = output_handler;
    output_add->data = this;
    wl_signal_add(&backend->events.new_output, output_add);

    // 输入管理
    auto input_handler = [](void *data, struct wl_listener *listener, void *device) {
        Server *server = static_cast<Server*>(data);
        auto *wlr_device = static_cast<struct wlr_input_device*>(device);
        Input *input = new Input(server, wlr_device);
        server->add_input(input);
    };

    struct wl_listener *input_add = wl_listener_create();
    input_add->notify = input_handler;
    input_add->data = this;
    wl_signal_add(&backend->events.new_input, input_add);

    // 初始化配置系统
    if (!init_config()) {
        utils::Log::error("Failed to initialize config system");
        return false;
    }

    // 初始化插件系统
    if (!init_plugins()) {
        utils::Log::error("Failed to initialize plugin system");
        return false;
    }

    utils::Log::info("Server initialized successfully");
    return true;
}

void Server::run() {
    if (!backend) {
        utils::Log::error("Backend not initialized");
        return;
    }

    if (!wlr_backend_start(backend)) {
        utils::Log::error("Failed to start backend");
        return;
    }

    utils::Log::info("Server running");
    utils::Log::info("Press Ctrl+C to exit");
    
    // 运行主循环
    wl_display_run(display);
}

void Server::shutdown() {
    utils::Log::info("Starting server shutdown...");
    
    // 清理窗口
    for (View *view : views) {
        delete view;
    }
    views.clear();
    
    // 清理配置系统
    if (lua_api) {
        delete lua_api;
        lua_api = nullptr;
    }
    if (config) {
        delete config;
        config = nullptr;
    }
    
    // 清理插件系统
    if (plugin_host) {
        delete plugin_host;
        plugin_host = nullptr;
    }
    
    // 清理输入设备
    for (Input *input : inputs) {
        delete input;
    }
    inputs.clear();
    
    // 清理输出
    for (Output *output : outputs) {
        delete output;
    }
    outputs.clear();
    
    // 销毁输出布局
    if (output_layout) {
        wlr_output_layout_destroy(output_layout);
        output_layout = nullptr;
    }
    
    // 停止后端
    if (backend) {
        utils::Log::info("Stopping backend...");
        wlr_backend_destroy(backend);
        backend = nullptr;
    }
    
    // 销毁xdg-shell
    if (xdg_shell) {
        wlr_xdg_shell_destroy(xdg_shell);
        xdg_shell = nullptr;
    }

    // 销毁显示
    if (display) {
        utils::Log::info("Destroying display...");
        wl_display_destroy(display);
        display = nullptr;
    }

    // 重置所有指针
    renderer = nullptr;
    compositor = nullptr;
    event_loop = nullptr;

    utils::Log::info("Server shutdown completed");
}

void Server::add_input(Input *input) {
    inputs.push_back(input);
}

void Server::remove_input(Input *input) {
    auto it = std::find(inputs.begin(), inputs.end(), input);
    if (it != inputs.end()) {
        inputs.erase(it);
    }
}

uint64_t Server::get_last_input_time() const {
    uint64_t last_time = 0;
    for (const Input *input : inputs) {
        uint64_t input_time = input->get_last_input_time();
        if (input_time > last_time) {
            last_time = input_time;
        }
    }
    return last_time;
}

wl_display *Server::get_display() const {
    return display;
}

struct wlr_backend *Server::get_backend() const {
    return backend;
}

struct wlr_renderer *Server::get_renderer() const {
    return renderer;
}

struct wlr_output_layout *Server::get_output_layout() const {
    return output_layout;
}

void Server::add_output(Output *output) {
    outputs.push_back(output);
    struct wlr_output *wlr_output = output->get_wlr_output();
    
    // 将输出添加到布局
    wlr_output_layout_add_auto(output_layout, wlr_output);
    
    utils::Log::info("Output added: " + output->get_name());
    utils::Log::info("Total outputs: " + std::to_string(outputs.size()));
}

void Server::remove_output(Output *output) {
    auto it = std::find(outputs.begin(), outputs.end(), output);
    if (it != outputs.end()) {
        outputs.erase(it);
        utils::Log::info("Output removed: " + output->get_name());
        utils::Log::info("Total outputs: " + std::to_string(outputs.size()));
    }
}

void Server::add_view(View *view) {
    views.push_back(view);
    utils::Log::info("View added");
    utils::Log::info("Total views: " + std::to_string(views.size()));
}

void Server::remove_view(View *view) {
    auto it = std::find(views.begin(), views.end(), view);
    if (it != views.end()) {
        views.erase(it);
        utils::Log::info("View removed");
        utils::Log::info("Total views: " + std::to_string(views.size()));
    }
}

const std::vector<View*>& Server::get_views() const {
    return views;
}

bool Server::init_config() {
    // 创建配置对象
    config = new Config();
    if (!config) {
        utils::Log::error("Failed to create Config object");
        return false;
    }

    // 创建Lua API对象
    lua_api = new LuaAPI(this);
    if (!lua_api) {
        utils::Log::error("Failed to create LuaAPI object");
        return false;
    }

    // 初始化Lua API
    if (!lua_api->init()) {
        utils::Log::error("Failed to initialize Lua API");
        return false;
    }

    // 加载默认配置
    load_config("config.lua");

    utils::Log::info("Config system initialized");
    return true;
}

bool Server::load_config(const std::string &path) {
    if (!lua_api) {
        return false;
    }

    return lua_api->load_script(path);
}

bool Server::reload_config() {
    utils::Log::info("Reloading config...");
    return load_config("config.lua");
}

Config *Server::get_config() const {
    return config;
}

LuaAPI *Server::get_lua_api() const {
    return lua_api;
}

bool Server::init_plugins() {
    // 创建插件宿主对象
    plugin_host = new PluginHost(this);
    if (!plugin_host) {
        utils::Log::error("Failed to create PluginHost object");
        return false;
    }

    // 初始化插件宿主
    if (!plugin_host->init()) {
        utils::Log::error("Failed to initialize plugin host");
        return false;
    }

    utils::Log::info("Plugin system initialized");
    return true;
}

bool Server::start_plugin(const std::string &path) {
    if (!plugin_host) {
        return false;
    }

    return plugin_host->start_plugin(path);
}

void Server::stop_all_plugins() {
    if (plugin_host) {
        plugin_host->stop_all_plugins();
    }
}

PluginHost *Server::get_plugin_host() const {
    return plugin_host;
}

} // namespace aki

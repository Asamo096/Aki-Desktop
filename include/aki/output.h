#pragma once

#include <wlroots.h>
#include <string>

namespace aki {

class Server;

namespace render {
    class Renderer;
}

class Output {
private:
    Server *server;
    struct wlr_output *wlr_output;
    std::string name;
    struct wlr_output_layout *layout;
    int scale;
    render::Renderer *renderer;
    
    // 监听器
    struct wl_listener destroy_listener;
    struct wl_listener mode_listener;
    struct wl_listener scale_listener;

public:
    Output(Server *server, struct wlr_output *wlr_output);
    ~Output();

    void init();
    void frame();
    void handle_destroy();
    void handle_mode();
    void handle_scale();

    struct wlr_output *get_wlr_output() const;
    const std::string &get_name() const;
    int get_scale() const;
};

} // namespace aki

#pragma once

#include <wlroots.h>

namespace aki {

class Server;
class View;

namespace render {

class Effects;

class Renderer {
private:
    Server *server;
    struct wlr_renderer *wlr_renderer;
    Effects *effects;

public:
    Renderer(Server *server);
    ~Renderer();

    bool init();
    void begin(struct wlr_output *output);
    void end(struct wlr_output *output);
    void clear(const float color[4]);
    void render_view(View *view, int x, int y);

    struct wlr_renderer *get_wlr_renderer() const;
    Effects *get_effects() const;
};

} // namespace render
} // namespace aki

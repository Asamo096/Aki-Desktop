#pragma once

#include <wlroots.h>

namespace aki {

class Server;

class View {
private:
    Server *server;
    struct wlr_surface *surface;
public:
    struct wlr_xdg_surface *xdg_surface;

public:
    View(Server *server, struct wlr_surface *surface);
    ~View();

    void map();
    void unmap();
    void render(struct wlr_renderer *renderer, int x, int y);

    struct wlr_surface *get_surface() const;
};

} // namespace aki

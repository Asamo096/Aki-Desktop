#include "aki/view.h"
#include "aki/server.h"
#include "aki/utils/log.h"

#include <wlroots.h>

namespace aki {

View::View(Server *server, struct wlr_surface *surface) :
    server(server),
    surface(surface),
    xdg_surface(nullptr) {
}

View::~View() {
}

void View::map() {
    utils::Log::info("View mapped");
}

void View::unmap() {
    utils::Log::info("View unmapped");
}

void View::render(struct wlr_renderer *renderer, int x, int y) {
    if (!surface) {
        return;
    }

    struct wlr_box box = {x, y, 0, 0};
    wlr_surface_get_size(surface, &box.width, &box.height);

    wlr_render_surface(renderer, surface, &box, 1.0f);
}

struct wlr_surface *View::get_surface() const {
    return surface;
}

} // namespace aki

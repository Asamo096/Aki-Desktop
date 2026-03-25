#include "aki/render/renderer.h"
#include "aki/render/effects.h"
#include "aki/server.h"
#include "aki/view.h"
#include "aki/utils/log.h"

#include <wlroots.h>

namespace aki {

namespace render {

Renderer::Renderer(Server *server) :
    server(server),
    wlr_renderer(nullptr),
    effects(nullptr) {
}

Renderer::~Renderer() {
    if (effects) {
        delete effects;
    }
}

bool Renderer::init() {
    wlr_renderer = server->get_renderer();
    if (!wlr_renderer) {
        utils::Log::error("Failed to get wlr renderer");
        return false;
    }

    // 创建和初始化效果系统
    effects = new Effects();
    effects->init();

    utils::Log::info("Renderer initialized");
    return true;
}

void Renderer::begin(struct wlr_output *output) {
    if (!wlr_renderer) {
        return;
    }

    wlr_output_make_current(output, nullptr);
    int width, height;
    wlr_output_effective_resolution(output, &width, &height);
    wlr_renderer_begin(wlr_renderer, width, height);
}

void Renderer::end(struct wlr_output *output) {
    if (!wlr_renderer) {
        return;
    }

    wlr_renderer_end(wlr_renderer);
    wlr_output_swap_buffers(output, nullptr, nullptr);
}

void Renderer::clear(const float color[4]) {
    if (!wlr_renderer) {
        return;
    }

    wlr_renderer_clear(wlr_renderer, color);
}

struct wlr_renderer *Renderer::get_wlr_renderer() const {
    return wlr_renderer;
}

Effects *Renderer::get_effects() const {
    return effects;
}

void Renderer::render_view(View *view, int x, int y) {
    if (!wlr_renderer || !view) {
        return;
    }

    // 应用圆角效果
    effects->apply_rounded_corners();

    // 应用阴影效果
    effects->apply_shadow();

    // 渲染窗口
    view->render(wlr_renderer, x, y);
}

} // namespace render
} // namespace aki

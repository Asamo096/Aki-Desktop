#include "aki/output.h"
#include "aki/server.h"
#include "aki/view.h"
#include "aki/render/renderer.h"
#include "aki/utils/log.h"

#include <wlroots.h>

namespace aki {

Output::Output(Server *server, struct wlr_output *wlr_output) :
    server(server),
    wlr_output(wlr_output),
    name(""),
    layout(nullptr),
    scale(1),
    renderer(nullptr) {
    if (wlr_output->name) {
        name = wlr_output->name;
    }
    init();
}

Output::~Output() {
    // 清理监听器
    wl_list_remove(&destroy_listener.link);
    wl_list_remove(&mode_listener.link);
    wl_list_remove(&scale_listener.link);
    
    // 清理渲染器
    if (renderer) {
        delete renderer;
    }
}

void Output::init() {
    // 设置默认缩放
    wlr_output_set_scale(wlr_output, 1.0f);

    // 配置输出模式
    struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
    if (mode) {
        wlr_output_set_mode(wlr_output, mode);
        utils::Log::info("Set preferred mode for " + name + ": " + 
            std::to_string(mode->width) + "x" + std::to_string(mode->height) + 
            "@" + std::to_string(mode->refresh) + "Hz");
    }

    // 帧回调
    auto frame_handler = [](void *data, struct wl_listener *listener, void *unused) {
        Output *output = static_cast<Output*>(data);
        output->frame();
    };

    struct wl_listener *frame = wl_listener_create();
    frame->notify = frame_handler;
    frame->data = this;
    wl_signal_add(&wlr_output->events.frame, frame);

    // 销毁回调
    destroy_listener.notify = [](void *data, struct wl_listener *listener, void *unused) {
        Output *output = static_cast<Output*>(data);
        output->handle_destroy();
    };
    destroy_listener.data = this;
    wl_signal_add(&wlr_output->events.destroy, &destroy_listener);

    // 模式变化回调
    mode_listener.notify = [](void *data, struct wl_listener *listener, void *unused) {
        Output *output = static_cast<Output*>(data);
        output->handle_mode();
    };
    mode_listener.data = this;
    wl_signal_add(&wlr_output->events.mode, &mode_listener);

    // 缩放变化回调
    scale_listener.notify = [](void *data, struct wl_listener *listener, void *unused) {
        Output *output = static_cast<Output*>(data);
        output->handle_scale();
    };
    scale_listener.data = this;
    wl_signal_add(&wlr_output->events.scale, &scale_listener);

    // 创建和初始化渲染器
    renderer = new render::Renderer(server);
    if (renderer->init()) {
        utils::Log::info("Renderer initialized for output: " + name);
    } else {
        utils::Log::error("Failed to initialize renderer for output: " + name);
    }

    utils::Log::info("Output initialized: " + name);
}

void Output::frame() {
    // 渲染逻辑
    if (!renderer) {
        return;
    }

    // 开始渲染
    renderer->begin(wlr_output);

    // 清空背景
    float color[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    renderer->clear(color);

    // 渲染所有窗口
    const auto &views = server->get_views();
    int x = 10, y = 10;
    for (const View *view : views) {
        // 使用渲染器渲染窗口，应用效果
        renderer->render_view(const_cast<View*>(view), x, y);
        x += 100;
        y += 50;
        if (x > 800) {
            x = 10;
            y += 100;
        }
    }

    // 结束渲染
    renderer->end(wlr_output);
}

void Output::handle_destroy() {
    utils::Log::info("Output destroyed: " + name);
    server->remove_output(this);
    delete this;
}

void Output::handle_mode() {
    struct wlr_output_mode *mode = wlr_output_get_current_mode(wlr_output);
    if (mode) {
        utils::Log::info("Output mode changed for " + name + ": " + 
            std::to_string(mode->width) + "x" + std::to_string(mode->height) + 
            "@" + std::to_string(mode->refresh) + "Hz");
    }
}

void Output::handle_scale() {
    scale = static_cast<int>(wlr_output->scale);
    utils::Log::info("Output scale changed for " + name + ": " + std::to_string(scale));
}

struct wlr_output *Output::get_wlr_output() const {
    return wlr_output;
}

const std::string &Output::get_name() const {
    return name;
}

int Output::get_scale() const {
    return scale;
}

} // namespace aki

#pragma once

#include <wlroots.h>

namespace aki {

class Server;

class Input {
private:
    Server *server;
    struct wlr_input_device *device;
    
    // 键盘相关
    struct wlr_keyboard *keyboard;
    struct wlr_keyboard_group *keyboard_group;
    struct wl_listener key_signal;
    struct wl_listener modifiers_signal;
    
    // 指针相关
    struct wlr_pointer *pointer;
    struct wl_listener pointer_motion;
    struct wl_listener pointer_motion_absolute;
    struct wl_listener pointer_button;
    struct wl_listener pointer_axis;
    
    // 输入延迟测量
    uint64_t last_input_time;
    
    // 热键处理
    bool handle_keybindings(uint32_t keycode, uint32_t state);
    
public:
    Input(Server *server, struct wlr_input_device *device);
    ~Input();

    void init();

    struct wlr_input_device *get_device() const;
    
    // 输入延迟相关
    uint64_t get_last_input_time() const;
    void update_input_time();
};

} // namespace aki

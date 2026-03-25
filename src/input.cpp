#include "aki/input.h"
#include "aki/server.h"
#include "aki/utils/log.h"
#include "aki/utils/time.h"

#include <wlroots.h>
#include <xkbcommon/xkbcommon.h>
#include <cstdlib>

namespace aki {

// 键盘事件回调
static void key_handler(void *data, struct wl_listener *listener, void *event) {
    Input *input = static_cast<Input*>(data);
    auto *keyboard_event = static_cast<struct wlr_event_keyboard_key*>(event);
    
    // 更新输入时间
    input->update_input_time();
    
    // 处理热键
    bool handled = input->handle_keybindings(keyboard_event->keycode, keyboard_event->state);
    if (handled) {
        return;
    }
    
    // 其他键盘事件处理...
}

// 修饰键事件回调
static void modifiers_handler(void *data, struct wl_listener *listener, void *event) {
    // 修饰键处理...
}

// 指针移动事件回调
static void pointer_motion_handler(void *data, struct wl_listener *listener, void *event) {
    Input *input = static_cast<Input*>(data);
    // 更新输入时间
    input->update_input_time();
    
    // 鼠标移动处理...
}

// 指针绝对移动事件回调
static void pointer_motion_absolute_handler(void *data, struct wl_listener *listener, void *event) {
    Input *input = static_cast<Input*>(data);
    // 更新输入时间
    input->update_input_time();
    
    // 绝对鼠标移动处理...
}

// 指针按钮事件回调
static void pointer_button_handler(void *data, struct wl_listener *listener, void *event) {
    Input *input = static_cast<Input*>(data);
    // 更新输入时间
    input->update_input_time();
    
    // 鼠标按钮处理...
}

// 指针滚轮事件回调
static void pointer_axis_handler(void *data, struct wl_listener *listener, void *event) {
    Input *input = static_cast<Input*>(data);
    // 更新输入时间
    input->update_input_time();
    
    // 鼠标滚轮处理...
}

Input::Input(Server *server, struct wlr_input_device *device) :
    server(server),
    device(device),
    keyboard(nullptr),
    keyboard_group(nullptr),
    pointer(nullptr),
    last_input_time(0) {
    init();
}

Input::~Input() {
    // 清理监听器
    if (keyboard) {
        wl_list_remove(&key_signal.link);
        wl_list_remove(&modifiers_signal.link);
    }
    
    if (pointer) {
        wl_list_remove(&pointer_motion.link);
        wl_list_remove(&pointer_motion_absolute.link);
        wl_list_remove(&pointer_button.link);
        wl_list_remove(&pointer_axis.link);
    }
}

void Input::init() {
    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
        {
            keyboard = wlr_input_device_get_keyboard(device);
            if (!keyboard) {
                utils::Log::error("Failed to get keyboard device");
                return;
            }
            
            // 配置XKB
            struct xkb_rule_names rules = {"", "", "", "", ""};
            struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
            struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
            wlr_keyboard_set_keymap(keyboard, keymap);
            xkb_keymap_unref(keymap);
            xkb_context_unref(context);
            
            // 设置重复按键参数
            wlr_keyboard_set_repeat_info(keyboard, 25, 600);
            
            // 注册事件监听器
            key_signal.notify = key_handler;
            key_signal.data = this;
            wl_signal_add(&keyboard->events.key, &key_signal);
            
            modifiers_signal.notify = modifiers_handler;
            modifiers_signal.data = this;
            wl_signal_add(&keyboard->events.modifiers, &modifiers_signal);
            
            utils::Log::info("Keyboard input device initialized");
        }
        break;
        
    case WLR_INPUT_DEVICE_POINTER:
        {
            pointer = wlr_input_device_get_pointer(device);
            if (!pointer) {
                utils::Log::error("Failed to get pointer device");
                return;
            }
            
            // 注册事件监听器
            pointer_motion.notify = pointer_motion_handler;
            pointer_motion.data = this;
            wl_signal_add(&pointer->events.motion, &pointer_motion);
            
            pointer_motion_absolute.notify = pointer_motion_absolute_handler;
            pointer_motion_absolute.data = this;
            wl_signal_add(&pointer->events.motion_absolute, &pointer_motion_absolute);
            
            pointer_button.notify = pointer_button_handler;
            pointer_button.data = this;
            wl_signal_add(&pointer->events.button, &pointer_button);
            
            pointer_axis.notify = pointer_axis_handler;
            pointer_axis.data = this;
            wl_signal_add(&pointer->events.axis, &pointer_axis);
            
            utils::Log::info("Pointer input device initialized");
        }
        break;
        
    case WLR_INPUT_DEVICE_TOUCH:
        utils::Log::info("Touch input device initialized");
        break;
        
    case WLR_INPUT_DEVICE_TABLET_TOOL:
        utils::Log::info("Tablet tool input device initialized");
        break;
        
    case WLR_INPUT_DEVICE_TABLET_PAD:
        utils::Log::info("Tablet pad input device initialized");
        break;
        
    case WLR_INPUT_DEVICE_SWITCH:
        utils::Log::info("Switch input device initialized");
        break;
        
    default:
        utils::Log::info("Unknown input device initialized");
        break;
    }
}

bool Input::handle_keybindings(uint32_t keycode, uint32_t state) {
    if (state != WLR_KEY_PRESSED) {
        return false;
    }
    
    // Esc 键退出
    if (keycode == XKB_KEY_Escape) {
        utils::Log::info("Escape key pressed, exiting...");
        server->shutdown();
        return true;
    }
    
    // Mod+Enter 开终端
    uint32_t mods = keyboard ? wlr_keyboard_get_modifiers(keyboard) : 0;
    if ((mods & WLR_MODIFIER_LOGO) && keycode == XKB_KEY_Return) {
        utils::Log::info("Mod+Enter pressed, opening terminal...");
        // 启动终端
        const char *terminal = getenv("TERMINAL");
        if (!terminal) {
            terminal = "foot";
        }
        
        if (fork() == 0) {
            execlp(terminal, terminal, nullptr);
            exit(1);
        }
        
        return true;
    }
    
    // Mod+R 重新加载配置
    if ((mods & WLR_MODIFIER_LOGO) && keycode == XKB_KEY_r) {
        utils::Log::info("Mod+R pressed, reloading config...");
        server->reload_config();
        return true;
    }
    
    return false;
}

struct wlr_input_device *Input::get_device() const {
    return device;
}

uint64_t Input::get_last_input_time() const {
    return last_input_time;
}

void Input::update_input_time() {
    last_input_time = utils::Time::get_monotonic_us();
}

} // namespace aki

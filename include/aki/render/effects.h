#pragma once

namespace aki {

namespace render {

class Effects {
private:
    // 效果相关成员变量

public:
    Effects();
    ~Effects();

    void init();
    void apply_blur();
    void apply_shadow();
    void apply_rounded_corners();
};

} // namespace render
} // namespace aki

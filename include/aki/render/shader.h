#pragma once

#include <string>

namespace aki {

namespace render {

class Shader {
private:
    unsigned int program;
    std::string vertex_shader;
    std::string fragment_shader;

public:
    Shader(const std::string &vertex_path, const std::string &fragment_path);
    ~Shader();

    bool compile();
    void use();
    void set_int(const std::string &name, int value);
    void set_float(const std::string &name, float value);
    void set_vec3(const std::string &name, float x, float y, float z);
    void set_vec4(const std::string &name, float x, float y, float z, float w);

    unsigned int get_program() const;
};

} // namespace render
} // namespace aki

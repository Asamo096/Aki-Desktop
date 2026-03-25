#include "aki/render/shader.h"
#include "aki/utils/log.h"

#include <fstream>
#include <sstream>
#include <GL/glew.h>

namespace aki {

namespace render {

Shader::Shader(const std::string &vertex_path, const std::string &fragment_path) :
    program(0) {
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(fragment_path);

    std::stringstream vertex_stream, fragment_stream;
    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();

    vertex_shader = vertex_stream.str();
    fragment_shader = fragment_stream.str();
}

Shader::~Shader() {
    if (program) {
        glDeleteProgram(program);
    }
}

bool Shader::compile() {
    unsigned int vertex, fragment;

    // 编译顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char *v_src = vertex_shader.c_str();
    glShaderSource(vertex, 1, &v_src, nullptr);
    glCompileShader(vertex);

    // 编译片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char *f_src = fragment_shader.c_str();
    glShaderSource(fragment, 1, &f_src, nullptr);
    glCompileShader(fragment);

    // 链接程序
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    // 清理
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return true;
}

void Shader::use() {
    if (program) {
        glUseProgram(program);
    }
}

void Shader::set_int(const std::string &name, int value) {
    if (program) {
        glUniform1i(glGetUniformLocation(program, name.c_str()), value);
    }
}

void Shader::set_float(const std::string &name, float value) {
    if (program) {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }
}

void Shader::set_vec3(const std::string &name, float x, float y, float z) {
    if (program) {
        glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
    }
}

void Shader::set_vec4(const std::string &name, float x, float y, float z, float w) {
    if (program) {
        glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
    }
}

unsigned int Shader::get_program() const {
    return program;
}

} // namespace render
} // namespace aki

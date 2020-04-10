#pragma once

#include <glad/glad.h>

#include <fstream>
#include <glpp/idhandle.hpp>
#include <iostream>
#include <string>

namespace glpp {
enum ShaderType : GLenum {
  VERTEX_SHADER = GL_VERTEX_SHADER,
  GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
  FRAGMENT_SHADER = GL_FRAGMENT_SHADER
};

class Shader {
public:
  explicit Shader(ShaderType type) { handle_.id = glCreateShader(type); }

  Shader(ShaderType type, const std::string &source) : Shader(type) {
    SetSource(source);
    Compile();
  }

  Shader(Shader &&other) = default;

  Shader &operator=(Shader &&other) = default;

  [[nodiscard]] GLuint Id() const { return handle_.id; }

  void SetSource(const std::string &source) {
    const auto source_str = source.c_str();
    glShaderSource(Id(), 1, &source_str, nullptr);
  }

  void Compile() {
    glCompileShader(Id());

    // Check Shader.
    auto result = GL_FALSE;
    glGetShaderiv(Id(), GL_COMPILE_STATUS, &result);
    int log_length;
    glGetShaderiv(Id(), GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
      std::string msg(log_length, '\0');
      glGetShaderInfoLog(Id(), log_length, nullptr, msg.data());
      std::clog << "Shader compilation log: " << msg << std::endl;
    }

    if (result != GL_TRUE) {
      throw std::runtime_error("Shader compilation error");
    }
  }

  static Shader FromFile(ShaderType type, const std::string &file_path) {
    std::string source;
    std::ifstream stream(file_path, std::ios::in);
    if (stream.is_open()) {
      std::string line;
      while (getline(stream, line)) source += "\n" + line;
      stream.close();
    } else {
      throw std::runtime_error("Cannot open " + file_path);
    }
    return Shader(type, source);
  }

private:
  static void Delete(GLuint id) { glDeleteShader(id); }

  IdHandle<Delete> handle_;
};
}  // namespace glpp

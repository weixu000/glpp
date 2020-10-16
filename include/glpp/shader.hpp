#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "details/object.hpp"
#include "gl.h"

namespace glpp {
enum class ShaderStage : GLenum {
  VERTEX_SHADER = GL_VERTEX_SHADER,
  GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
  FRAGMENT_SHADER = GL_FRAGMENT_SHADER
};

namespace details {
template <ShaderStage stage>
struct ShaderTrait {
  static GLuint Create() { return glCreateShader(static_cast<GLenum>(stage)); }

  static void Delete(GLuint id) { glDeleteShader(id); }
};
}  // namespace details

template <ShaderStage stage>
class Shader : public details::Object<details::ShaderTrait<stage>> {
 public:
  Shader() = default;

  explicit Shader(const std::string &source) {
    SetSource(source);
    Compile();
  }

  using details::Object<details::ShaderTrait<stage>>::Id;

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

  static Shader FromFile(const std::string &file_path) {
    std::string source;
    std::ifstream stream(file_path, std::ios::in);
    if (stream.is_open()) {
      std::string line;
      while (getline(stream, line)) source += "\n" + line;
      stream.close();
    } else {
      throw std::runtime_error("Cannot open " + file_path);
    }
    return Shader(source);
  }
};

using VertexShader = Shader<ShaderStage::VERTEX_SHADER>;
using GeometryShader = Shader<ShaderStage::GEOMETRY_SHADER>;
using FragmentShader = Shader<ShaderStage::FRAGMENT_SHADER>;
}  // namespace glpp

#pragma once

#include <glad/glad.h>

#include <glpp/idhandle.hpp>
#include <glpp/shader.hpp>
#include <string>
#include <tuple>
#include <unordered_map>

namespace glpp {
class Program {
public:
  Program() { handle_.id = glCreateProgram(); }

  template <typename... Shader>
  explicit Program(const Shader &... s) : Program() {
    Attach(s...);
    Link();
    Detach(s...);
  }

  Program(Program &&other) = default;

  Program &operator=(Program &&other) = default;

  [[nodiscard]] GLuint Id() const { return handle_.id; }

  void Use() { glUseProgram(Id()); }

  void Attach(const Shader &shader) { glAttachShader(Id(), shader.Id()); }

  template <typename... Shaders>
  void Attach(const Shader &s, const Shaders &... ss) {
    Attach(s);
    (Attach(ss), ...);
  }

  void Detach(const Shader &shader) { glDetachShader(Id(), shader.Id()); }

  template <typename... Shaders>
  void Detach(const Shader &s, const Shaders &... ss) {
    Detach(s);
    (Detach(ss), ...);
  }

  void Link() {
    glLinkProgram(Id());

    // Check the program.
    const auto log_length = GetParam(GL_INFO_LOG_LENGTH);
    if (log_length > 0) {
      std::string msg(log_length, '\0');
      glGetProgramInfoLog(Id(), log_length, nullptr, msg.data());
      std::clog << "Program linking log: " << msg << std::endl;
    }

    if (GetParam(GL_LINK_STATUS) != GL_TRUE) {
      throw std::runtime_error("Program link error");
    }

    uniform_locs_ =
        GetLocations<GL_ACTIVE_UNIFORMS, GL_ACTIVE_UNIFORM_MAX_LENGTH>(
            glGetActiveUniform, glGetUniformLocation);
    attrib_locs_ =
        GetLocations<GL_ACTIVE_ATTRIBUTES, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH>(
            glGetActiveAttrib, glGetAttribLocation);
  }

  GLint GetParam(GLenum pname) {
    GLint ret;
    glGetProgramiv(Id(), pname, &ret);
    return ret;
  }

  GLint UniformLoc(const std::string &name) { return uniform_locs_.at(name); }

  template <typename... Names>
  auto UniformLoc(const std::string &name, const Names &... names) {
    return std::make_tuple(uniform_locs_.at(name), uniform_locs_.at(names)...);
  }

  GLint AttribLoc(const std::string &name) { return attrib_locs_.at(name); }

  template <typename... Names>
  auto AttribLoc(const std::string &name, const Names &... names) {
    return std::make_tuple(attrib_locs_.at(name), attrib_locs_.at(names)...);
  }

  template <typename T>
  void Uniform(const std::string &name, const T &val);

private:
  /**
   * Get Uniform/Attribute location
   */
  template <GLenum GL_ACTIVE_X, GLenum GL_ACTIVE_X_MAX_LENGTH,
            typename glGetActiveX, typename glGetXLocation>
  std::unordered_map<std::string, GLint> GetLocations(glGetActiveX f1,
                                                      glGetXLocation f2) {
    std::unordered_map<std::string, GLint> locs;
    const auto count = GetParam(GL_ACTIVE_X);
    const auto max_len = GetParam(GL_ACTIVE_X_MAX_LENGTH);
    for (GLint i = 0; i < count; i++) {
      std::string name(max_len, '\0');
      GLsizei length;
      GLint size;
      GLenum type;
      f1(Id(), i, max_len, &length, &size, &type, name.data());
      name.resize(length);
      locs[name] = f2(Id(), name.c_str());
    }
    return locs;
  }

  static void Delete(GLuint id) { glDeleteProgram(id); }

  IdHandle<Delete> handle_;
  std::unordered_map<std::string, GLint> uniform_locs_, attrib_locs_;
};
}  // namespace glpp

#include <glpp/uniform.inc>

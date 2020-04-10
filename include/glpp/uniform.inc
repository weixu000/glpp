#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace glpp {
template <>
inline void Program::Uniform<int>(const std::string &name, const int &val) {
  glProgramUniform1i(Id(), uniform_locs_.at(name), val);
}

template <>
inline void Program::Uniform<float>(const std::string &name, const float &val) {
  glProgramUniform1f(Id(), uniform_locs_.at(name), val);
}

template <>
inline void Program::Uniform<glm::vec3>(const std::string &name,
                                        const glm::vec3 &val) {
  glProgramUniform3fv(Id(), uniform_locs_.at(name), 1, glm::value_ptr(val));
}

template <>
inline void Program::Uniform<glm::mat4>(const std::string &name,
                                        const glm::mat4 &val) {
  glProgramUniformMatrix4fv(Id(), uniform_locs_.at(name), 1, GL_FALSE,
                            glm::value_ptr(val));
}
}  // namespace glpp

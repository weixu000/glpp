#pragma once

#include <glm/glm.hpp>
#include <tuple>

#include "buffer.hpp"
#include "details/object.hpp"
#include "gl.h"

namespace glpp {
namespace details {
struct VertexArrayTrait {
  static GLuint Create() {
    GLuint id;
    glCreateVertexArrays(1, &id);
    return id;
  }

  static void Delete(GLuint id) { glDeleteVertexArrays(1, &id); }

  static void Bind(GLuint id) { glBindVertexArray(id); }
};
}  // namespace details

class VertexArray : public details::Object<details::VertexArrayTrait> {
 public:
  void BindElementBuffer(const Buffer &buffer) {
    glVertexArrayElementBuffer(Id(), buffer.Id());
  }

  void BindVertexBuffer(GLuint binding_index, const Buffer &buffer,
                        GLsizei stride, GLintptr offset = 0) {
    glVertexArrayVertexBuffer(Id(), binding_index, buffer.Id(), offset, stride);
  }

  void BindingDivisor(GLuint binding_index, GLuint divisor) {
    glVertexArrayBindingDivisor(Id(), binding_index, divisor);
  }

  template <typename... AttribIndices>
  void AttribBinding(GLuint binding_index, AttribIndices... attrib_indices) {
    static_assert(sizeof...(attrib_indices), "No attribute to bind");
    (glVertexArrayAttribBinding(Id(), attrib_indices, binding_index), ...);
  }

  template <typename... AttribIndices>
  void EnableAttrib(AttribIndices... attrib_indices) {
    static_assert(sizeof...(attrib_indices), "No attribute to enable");
    (glEnableVertexArrayAttrib(Id(), attrib_indices), ...);
  }

  void AttribFormat(GLuint attribindex, GLint size, GLenum type,
                    GLboolean normalized, GLuint relativeoffset) {
    glVertexArrayAttribFormat(Id(), attribindex, size, type, normalized,
                              relativeoffset);
  }

  template <typename T>
  void AttribFormat(GLuint attribindex, GLuint relativeoffset);

  void AttribIFormat(GLuint attribindex, GLint size, GLenum type,
                     GLuint relativeoffset) {
    glVertexArrayAttribIFormat(Id(), attribindex, size, type, relativeoffset);
  }

  template <typename T>
  void AttribIFormat(GLuint attribindex, GLuint relativeoffset);
};

#define ATTRIB_FORMAT_DEFINE(T, size, type, normalized)                \
  template <>                                                          \
  inline void VertexArray::AttribFormat<T>(GLuint attribindex,         \
                                           GLuint relativeoffset) {    \
    AttribFormat(attribindex, size, type, normalized, relativeoffset); \
  }

ATTRIB_FORMAT_DEFINE(float, 1, GL_FLOAT, GL_FALSE)
ATTRIB_FORMAT_DEFINE(glm::vec2, 2, GL_FLOAT, GL_FALSE)
ATTRIB_FORMAT_DEFINE(glm::vec3, 3, GL_FLOAT, GL_FALSE)
ATTRIB_FORMAT_DEFINE(glm::vec4, 4, GL_FLOAT, GL_FALSE)

#undef ATTRIB_FORMAT_DEFINE

#define ATTRIB_I_FORMAT_DEFINE(T, size, type)                        \
  template <>                                                        \
  inline void VertexArray::AttribIFormat<T>(GLuint attribindex,      \
                                            GLuint relativeoffset) { \
    AttribIFormat(attribindex, size, type, relativeoffset);          \
  }

ATTRIB_I_FORMAT_DEFINE(glm::uint, 1, GL_UNSIGNED_INT)

#undef ATTRIB_I_FORMAT_DEFINE
}  // namespace glpp

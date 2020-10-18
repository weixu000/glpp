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

  template <typename Tuple>
  void AttribFormat(GLuint bindingindex) {
    SetAttribFormatFromTupleImpl<Tuple>(
        bindingindex, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  }

 private:
  template <typename Tuple, std::size_t... Is>
  void SetAttribFormatFromTupleImpl(GLuint bindingindex,
                                    std::index_sequence<Is...>) {
    (AttribBinding(bindingindex, Is), ...);
    GLuint offset = sizeof(Tuple);
    ((offset -= sizeof(std::tuple_element_t<Is, Tuple>),
      AttribFormat<std::tuple_element_t<Is, Tuple>>(Is, offset)),
     ...);
  }
};

template <>
inline void VertexArray::AttribFormat<float>(GLuint attribindex,
                                             GLuint relativeoffset) {
  AttribFormat(attribindex, 1, GL_FLOAT, GL_FALSE, relativeoffset);
}

template <>
inline void VertexArray::AttribFormat<glm::vec2>(GLuint attribindex,
                                                 GLuint relativeoffset) {
  AttribFormat(attribindex, 2, GL_FLOAT, GL_FALSE, relativeoffset);
}

template <>
inline void VertexArray::AttribFormat<glm::vec3>(GLuint attribindex,
                                                 GLuint relativeoffset) {
  AttribFormat(attribindex, 3, GL_FLOAT, GL_FALSE, relativeoffset);
}

template <>
inline void VertexArray::AttribFormat<glm::vec4>(GLuint attribindex,
                                                 GLuint relativeoffset) {
  AttribFormat(attribindex, 4, GL_FLOAT, GL_FALSE, relativeoffset);
}
}  // namespace glpp

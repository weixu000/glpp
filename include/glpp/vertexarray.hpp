#pragma once

#include <glm/glm.hpp>
#include <tuple>

#include "buffer.hpp"
#include "details/idhandle.hpp"
#include "gl.h"

namespace glpp {
class VertexArray {
 public:
  VertexArray() { glCreateVertexArrays(1, &handle_.id); }

  VertexArray(VertexArray &&other) = default;

  VertexArray &operator=(VertexArray &&other) = default;

  [[nodiscard]] GLuint Id() const { return handle_.id; }

  void Bind() { glBindVertexArray(Id()); }

  void BindElementBuffer(const Buffer &buffer) {
    glVertexArrayElementBuffer(Id(), buffer.Id());
  }

  void BindVertexBuffer(GLuint bindingindex, const Buffer &buffer,
                        GLsizei stride, GLintptr offset = 0) {
    glVertexArrayVertexBuffer(Id(), bindingindex, buffer.Id(), offset, stride);
  }

  void AttribBinding(GLuint bindingindex, GLuint attribindex) {
    glVertexArrayAttribBinding(Id(), attribindex, bindingindex);
  }

  template <typename... AttribIndices>
  void AttribBinding(GLuint bindingindex, GLuint attribindex,
                     AttribIndices... attrib_indices) {
    AttribBinding(bindingindex, attribindex);
    (AttribBinding(bindingindex, attrib_indices), ...);
  }

  void EnableAttrib(GLuint attribindex) {
    glEnableVertexArrayAttrib(Id(), attribindex);
  }

  template <typename... AttribIndices>
  void EnableAttrib(GLuint attribindex, AttribIndices... attrib_indices) {
    EnableAttrib(attribindex);
    (EnableAttrib(attrib_indices), ...);
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

  static void Delete(GLuint id) { glDeleteVertexArrays(1, &id); }

  details::IdHandle<Delete> handle_;
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

}  // namespace glpp

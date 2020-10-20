#pragma once

#include "details/object.hpp"
#include "gl.h"

namespace glpp {
enum class BufferTarget : GLenum {
  ARRAY_BUFFER = GL_ARRAY_BUFFER,
  ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER
};

namespace details {
struct BufferTrait {
  static GLuint Create() {
    GLuint id;
    glCreateBuffers(1, &id);
    return id;
  }

  static void Delete(GLuint id) { glDeleteBuffers(1, &id); }

  using Target = BufferTarget;

  static void Bind(Target target, GLuint id) {
    glBindBuffer(static_cast<GLenum>(target), id);
  }
};
}  // namespace details

class Buffer : public details::Object<details::BufferTrait> {
 public:
  void CreateStorage(GLsizeiptr size, const void *data = nullptr,
                     GLbitfield flags = 0) {
    glNamedBufferStorage(Id(), size, data, flags);
  }

  // For STL containers
  template <typename Container>
  void CreateStorage(const Container &arr, GLbitfield flags = 0) {
    CreateStorage(arr.size() * sizeof(typename Container::value_type),
                  arr.data(), flags);
  }

  void SetSubData(GLsizeiptr size, const void *data = nullptr,
                  GLintptr offset = 0) {
    glNamedBufferSubData(Id(), offset, size, data);
  }

  // For STL containers
  template <typename Container>
  void SetSubData(const Container &arr, GLintptr offset = 0) {
    SetSubData(arr.size() * sizeof(typename Container::value_type), arr.data(),
               offset);
  }

  void *MapRange(GLintptr offset, GLsizeiptr length, GLbitfield access) {
    return glMapNamedBufferRange(Id(), offset, length, access);
  }

  void Unmap() { glUnmapNamedBuffer(Id()); }

  void InvalidateData() { glInvalidateBufferData(Id()); }
};
}  // namespace glpp

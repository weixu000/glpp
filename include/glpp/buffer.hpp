#pragma once

#include <glad/glad.h>

#include <glpp/idhandle.hpp>

namespace glpp {
enum BufferTarget : GLenum {
  ARRAY_BUFFER = GL_ARRAY_BUFFER,
  ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER
};

class Buffer {
public:
  Buffer() { glCreateBuffers(1, &handle_.id); }

  Buffer(Buffer &&other) = default;

  Buffer &operator=(Buffer &&other) = default;

  [[nodiscard]] GLuint Id() const { return handle_.id; }

  void Bind(BufferTarget target) { glBindBuffer(target, Id()); }

  void CreateStorage(GLsizeiptr size, const void *data, GLbitfield flags) {
    glNamedBufferStorage(Id(), size, data, flags);
  }

  // For STL containers
  template <typename Container>
  void CreateStorage(const Container &arr, GLbitfield flags) {
    CreateStorage(arr.size() * sizeof(typename Container::value_type),
                  arr.data(), flags);
  }

  void SetSubData(GLsizeiptr size, const void *data, GLintptr offset = 0) {
    glNamedBufferSubData(Id(), offset, size, data);
  }

  // For STL containers
  template <typename Container>
  void SetSubData(const Container &arr, GLintptr offset = 0) {
    SetSubData(arr.size() * sizeof(typename Container::value_type), arr.data(),
               offset);
  }

private:
  static void Delete(GLuint id) { glDeleteBuffers(1, &id); }

  IdHandle<Delete> handle_;
};
}  // namespace glpp

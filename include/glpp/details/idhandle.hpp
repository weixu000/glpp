#pragma once

#include <cassert>
#include <utility>

#include "../gl.h"

namespace glpp::details {
using DeleteFuncType = void(GLuint);

/**
 * Implement move semantics and prohibit copying of OpenGL object
 * Take care of destruction but not creation
 * @tparam DeleteFunc GlDelete*
 */
template <DeleteFuncType DeleteFunc>
struct IdHandle {
  IdHandle() = default;

  IdHandle(IdHandle &&other) noexcept : id(other.id) { other.id = 0U; }

  ~IdHandle() { DeleteFunc(id); }

  IdHandle(const IdHandle &) = delete;

  IdHandle &operator=(const IdHandle &) = delete;

  IdHandle &operator=(IdHandle &&other) noexcept {
    assert(this != &other);
    // Use local variable to call destructor on old object
    IdHandle tmp(std::move(*this));
    std::swap(id, other.id);
    return *this;
  }

  GLuint id = 0U;
};
}  // namespace glpp::details

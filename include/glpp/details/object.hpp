#pragma once
#include <type_traits>
#include <utility>

#include "../gl.h"

namespace glpp::details {
template <typename Trait>
class Object {
 public:
  Object() : id_{Trait::Create()} {}

  Object(const Object &) = delete;

  Object(Object &&other) noexcept { std::swap(id_, other.id_); }

  ~Object() { Trait::Delete(id_); }

  Object &operator=(const Object &) = delete;

  Object &operator=(Object &&other) noexcept {
    if (this == &other) return *this;
    Object tmp(std::move(*this));
    std::swap(id_, other.id_);
    return *this;
  }

  [[nodiscard]] GLuint Id() const { return id_; }

  template <bool = true>
  void Bind() {
    Trait::Bind(id_);
  }

  template <typename Target>
  void Bind(Target target) {
    Trait::Bind(target, id_);
  }

 private:
  GLuint id_{0U};
};
}  // namespace glpp::details

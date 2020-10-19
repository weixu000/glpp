#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "details/object.hpp"
#include "gl.h"

namespace glpp {
enum class TextureType : GLenum {
  TEXTURE_1D = GL_TEXTURE_1D,
  TEXTURE_2D = GL_TEXTURE_2D,
  TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
};

namespace details {
template <TextureType type>
struct TextureTrait {
  static GLuint Create() {
    GLuint id;
    glCreateTextures(static_cast<GLenum>(type), 1, &id);
    return id;
  }

  static void Delete(GLuint id) { glDeleteTextures(1, &id); }

  using Target = TextureType;

  static void Bind(GLuint id) { glBindTexture(static_cast<GLenum>(type), id); }
};

template <typename Texture>
struct TextureUnitMixin {
  void BindUnit(GLuint unit) {
    glBindTextureUnit(unit, static_cast<Texture &>(*this).Id());
  }
};

template <typename Texture>
struct TextureMipmapMixin {
  void GenerateMipmap() {
    glGenerateMipmap(static_cast<Texture &>(*this).Id());
  }
};

template <typename Texture>
struct TextureFilteringMixin {
  void SetFilters(GLint min_filter, GLint mag_filter) {
    glTextureParameteri(static_cast<Texture &>(*this).Id(),
                        GL_TEXTURE_MIN_FILTER, min_filter);
    glTextureParameteri(static_cast<Texture &>(*this).Id(),
                        GL_TEXTURE_MAG_FILTER, mag_filter);
  }
};

template <typename Texture, GLuint... wraps>
struct TextureWrapMixin {
  template <typename... Ts>
  void SetWraps(Ts &&... params) {
    (glTextureParameteri(static_cast<Texture &>(*this).Id(), wraps, params),
     ...);
  }
};
}  // namespace details

class Texture1D
    : public details::Object<details::TextureTrait<TextureType::TEXTURE_1D>>,
      public details::TextureUnitMixin<Texture1D>,
      public details::TextureMipmapMixin<Texture1D>,
      public details::TextureFilteringMixin<Texture1D>,
      public details::TextureWrapMixin<Texture1D, GL_TEXTURE_WRAP_S> {
 public:
  void CreateStorage(GLsizei levels, GLenum internalformat, GLsizei width) {
    glTextureStorage1D(Id(), levels, internalformat, width);
  }

  void SetSubImage(GLint level, GLint xoffset, GLsizei width, GLenum format,
                   GLenum type, const void *pixels) {
    glTextureSubImage1D(Id(), level, xoffset, width, format, type, pixels);
  }
};

class Texture2D
    : public details::Object<details::TextureTrait<TextureType::TEXTURE_2D>>,
      public details::TextureUnitMixin<Texture2D>,
      public details::TextureMipmapMixin<Texture2D>,
      public details::TextureFilteringMixin<Texture2D>,
      public details::TextureWrapMixin<Texture2D, GL_TEXTURE_WRAP_S,
                                       GL_TEXTURE_WRAP_T> {
 public:
  void CreateStorage(GLsizei levels, GLenum internalformat, GLsizei width,
                     GLsizei height) {
    glTextureStorage2D(Id(), levels, internalformat, width, height);
  }

  void SetSubImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                   GLsizei height, GLenum format, GLenum type,
                   const void *pixels) {
    glTextureSubImage2D(Id(), level, xoffset, yoffset, width, height, format,
                        type, pixels);
  }
};

class TextureCubemap
    : public details::Object<
          details::TextureTrait<TextureType::TEXTURE_CUBE_MAP>>,
      public details::TextureUnitMixin<TextureCubemap>,
      public details::TextureMipmapMixin<TextureCubemap>,
      public details::TextureFilteringMixin<TextureCubemap> {
 public:
  void CreateStorage(GLsizei levels, GLenum internalformat, GLsizei size) {
    glTextureStorage2D(Id(), levels, internalformat, size, size);
  }

  void SetSubImage(GLint level, GLint xoffset, GLint yoffset, GLint face,
                   GLsizei width, GLsizei height, GLenum format, GLenum type,
                   const void *pixels) {
    glTextureSubImage3D(Id(), level, xoffset, yoffset, face, width, height, 1,
                        format, type, pixels);
  }
};
}  // namespace glpp

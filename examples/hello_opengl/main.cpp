#include <array>
#include <common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glpp/glpp.hpp>
#include <string>

#include "logo.hpp"

using namespace glm;
using namespace glpp;
using namespace std;

namespace {
struct Vertex {
  vec2 xy;
  vec2 tex;
};

/*
 * 3 -- 2
 * |    |
 * 0 -- 1
 */
const auto aspect_ratio = float(logo_width) / logo_height;
const std::array<Vertex, 4> vertices = {
    Vertex{{-aspect_ratio, -1.f}, {0.f, 0.f}},
    Vertex{{aspect_ratio, -1.f}, {1.f, 0.f}},
    Vertex{{aspect_ratio, 1.f}, {1.f, 1.f}},
    Vertex{{-aspect_ratio, 1.f}, {0.f, 1.f}}};

const std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

string vertex_shader_text = R"(
#version 450

layout(location = 0) in vec2 xy;
layout(location = 1) in vec2 tex;

out vec2 vTex;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(xy, 0.0, 1.0);
    vTex = tex;
}
)";

string fragment_shader_text = R"(
#version 450

in vec2 vTex;

uniform sampler2D logo;

out vec4 fragColor;

void main() {
    fragColor = texture(logo, vTex);
}
)";
}  // namespace

int main() {
  const auto window = SetupGL("Hello OpenGL");

  Buffer vbo, ebo;
  vbo.CreateStorage(vertices);
  ebo.CreateStorage(indices);

  Program program{VertexShader{vertex_shader_text},
                  FragmentShader{fragment_shader_text}};

  VertexArray vao;
  vao.BindElementBuffer(ebo);
  vao.BindVertexBuffer(0, vbo, sizeof(Vertex), 0);
  vao.EnableAttrib(0, 1);
  vao.AttribBinding(0, 0, 1);
  vao.AttribFormat<vec2>(0, 0);
  vao.AttribFormat<vec2>(1, sizeof(vec2));

  Texture2D texture;
  texture.CreateStorage(1, GL_RGBA4, logo_width, logo_height);
  texture.SetSubImage(0, 0, 0, logo_width, logo_height, GL_RGBA,
                      GL_UNSIGNED_SHORT_4_4_4_4, logo);
  texture.SetWraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  glActiveTexture(GL_TEXTURE0);
  texture.Bind();
  program.Uniform("logo", 0);

  program.Use();
  vao.Bind();

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    const float ratio = (float)width / height;
    mat4 m = rotate(mat4{1.f}, (float)glfwGetTime(), {0, 0, 1});
    mat4 p = ortho(-ratio * 2, ratio * 2, -2.f, 2.f, -1.f, 1.f);
    mat4 mvp = p * m;
    program.Uniform("MVP", mvp);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}
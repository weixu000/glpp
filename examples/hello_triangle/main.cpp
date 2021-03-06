#include <array>
#include <common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glpp/glpp.hpp>
#include <string>

namespace {
struct vertex_t {
  glm::vec2 xy;
  glm::vec3 rgb;
};
const std::array<vertex_t, 3> vertices = {
    vertex_t{{-0.6f, -0.4f}, {1.f, 0.f, 0.f}},
    vertex_t{{0.6f, -0.4f}, {0.f, 1.f, 0.f}},
    vertex_t{{0.f, 0.6f}, {0.f, 0.f, 1.f}}};

const char* vertex_shader_text = R"(
#version 110
uniform mat4 MVP;
attribute vec3 vCol;
attribute vec2 vPos;
varying vec3 color;
void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
}
)";

const char* fragment_shader_text = R"(
#version 110
varying vec3 color;
void main()
{
    gl_FragColor = vec4(color, 1.0);
}
)";
}  // namespace

/**
 * https://github.com/glfw/glfw/blob/master/examples/triangle-opengl.c
 */
int main() {
  using namespace glm;
  using namespace glpp;

  const auto window = SetupGL("Hello triangle");

  Buffer vertex_buffer;
  vertex_buffer.CreateStorage(vertices);

  Program program{VertexShader{vertex_shader_text},
                  FragmentShader{fragment_shader_text}};

  VertexArray vao;
  vao.BindVertexBuffer(0, vertex_buffer, sizeof(vertex_t), 0);
  const auto [vpos_location, vcol_location] = program.AttribLoc("vPos", "vCol");
  vao.EnableAttrib(vpos_location, vcol_location);
  vao.AttribBinding(0, vpos_location, vcol_location);
  vao.AttribFormat<glm::vec2>(vpos_location, 0);
  vao.AttribFormat<glm::vec3>(vcol_location, sizeof(glm::vec2));

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    const float ratio = (float)width / height;
    mat4 m = rotate(mat4{1.f}, (float)glfwGetTime(), {0, 0, 1});
    mat4 p = ortho(-ratio, ratio, -1.f, 1.f, -1.f, 1.f);
    mat4 mvp = p * m;

    program.Use();
    program.Uniform("MVP", mvp);
    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}
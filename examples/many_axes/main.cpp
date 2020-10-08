#include <array>
#include <common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>
#include <glpp/glpp.hpp>
#include <vector>

namespace {
const char* vertex_shader_source = R"(
#version 450

layout(location = 0) in mat4 model;

out mat4 vModel;

void main() {
  vModel = model;
}
)";

const char* geometry_shader_source = R"(
#version 450
layout(points) in;
layout(line_strip, max_vertices = 6) out;

in mat4 vModel[1];

uniform mat4 viewPersp;

out vec3 color;

void emitAxis(vec3 direction) {
  gl_Position = viewPersp*vModel[0]*vec4(0, 0, 0, 1);
  EmitVertex();
  gl_Position = viewPersp*vModel[0]*vec4(direction, 1);
  EmitVertex();
  EndPrimitive();
}

void main() {
  color = vec3(1, 0, 0);
  emitAxis(vec3(1, 0, 0));

  color = vec3(0, 1, 0);
  emitAxis(vec3(0, 1, 0));

  color = vec3(0, 0, 1);
  emitAxis(vec3(0, 0, 1));
}
)";

const char* fragment_shader_source = R"(
#version 450

in vec3 color;

void main()
{
    gl_FragColor = vec4(color, 1);
}
)";

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
}  // namespace

int main() {
  using namespace glm;
  using namespace glpp;
  using namespace std;

  const auto window = SetupGL("Many axes");
  glfwSetKeyCallback(window, key_callback);

  Program program{VertexShader{vertex_shader_source},
                  GeometryShader{geometry_shader_source},
                  FragmentShader{fragment_shader_source}};

  vector<mat4> transforms(10 * 10);
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      transforms[i * 10 + j] = translate(vec3{i * 1.2f - 6, j * 1.2f - 6, 0});
    }
  }

  vector<vec3> rotations(10 * 10);
  for (auto& r : rotations) {
    r = sphericalRand(1.0f);
  }

  Buffer vertex_buffer;
  vertex_buffer.CreateStorage(transforms, GL_DYNAMIC_STORAGE_BIT);
  VertexArray vao;
  vao.BindVertexBuffer(0, vertex_buffer, sizeof(mat4), 0);
  vao.EnableAttrib(0, 1, 2, 3);
  vao.AttribBinding(0, 0, 1, 2, 3);
  for (int i = 0; i < 4; ++i) {
    vao.AttribFormat<vec4>(i, i * sizeof(vec4));
  }

  program.Use();
  vao.Bind();

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    const mat4 p = ortho(-ratio * 6, ratio * 6, -6.f, 6.f, -2.f, 2.f);
    program.Uniform("viewPersp", p);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < 10 * 10; ++i) {
      transforms[i] = rotate(transforms[i], 0.01f, rotations[i]);
    }
    vertex_buffer.SetSubData(transforms);

    glDrawArrays(GL_POINTS, 0, 10 * 10);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}
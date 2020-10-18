#include <array>
#include <common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>
#include <glpp/glpp.hpp>
#include <string>
#include <vector>

#include "faces.hpp"

using namespace glm;
using namespace glpp;
using namespace std;

namespace {
const array<vec3, 8> vertices{vec3(-.5f, -.5f, -.5f), vec3(-.5f, .5f, -.5f),
                              vec3(.5f, -.5f, -.5f),  vec3(.5f, .5f, -.5f),
                              vec3(-.5f, -.5f, .5f),  vec3(-.5f, .5f, .5f),
                              vec3(.5f, -.5f, .5f),   vec3(.5f, .5f, .5f)};

const array<uint, 36> indices{0, 1, 2, 2, 1, 3, 7, 6, 3, 3, 6, 2,
                              7, 5, 6, 6, 5, 4, 4, 0, 6, 6, 0, 2,
                              1, 0, 5, 5, 0, 4, 3, 1, 7, 7, 1, 5};

string vertex_shader_source = R"(
#version 450

layout(location = 0) in mat4 model;
layout(location = 4) in vec3 pos;

uniform mat4 viewPersp;

out vec3 vTex;

void main() {
  gl_Position = viewPersp*model*vec4(pos, 1);
  vTex = pos;
}
)";

string fragment_shader_source = R"(
#version 450

in vec3 vTex;

uniform samplerCube faces;

out vec4 fragColor;

void main()
{
    fragColor = texture(faces, vTex);
}
)";
}  // namespace

int main() {
  const auto window = SetupGL("Many dices");

  Program program{VertexShader{vertex_shader_source},
                  FragmentShader{fragment_shader_source}};

  const auto grid = 7;
  vector<mat4> transforms(grid * grid);
  for (int i = 0; i < grid; ++i) {
    for (int j = 0; j < grid; ++j) {
      const auto p = vec3{(i - (float(grid) - 1) / 2) * 1.8f,
                          (j - (float(grid) - 1) / 2) * 1.8f, -10};
      transforms[i * grid + j] =
          inverse(lookAt(p, p + sphericalRand(1.0f), vec3{0, 1, 0}));
    }
  }
  vector<vec3> rotations(grid * grid);
  for (auto& r : rotations) {
    r = sphericalRand(1.0f);
  }
  Buffer transform_buffer;
  transform_buffer.CreateStorage(transforms, GL_DYNAMIC_STORAGE_BIT);

  Buffer vbo;
  vbo.CreateStorage(vertices);

  Buffer ebo;
  ebo.CreateStorage(indices);

  VertexArray vao;
  vao.BindElementBuffer(ebo);
  vao.BindVertexBuffer(0, transform_buffer, sizeof(mat4));
  vao.BindingDivisor(0, 1);
  vao.EnableAttrib(0, 1, 2, 3);
  vao.AttribBinding(0, 0, 1, 2, 3);
  for (int i = 0; i < 4; ++i) {
    vao.AttribFormat<vec4>(i, i * sizeof(vec4));
  }

  vao.BindVertexBuffer(1, vbo, sizeof(vec3));
  vao.EnableAttrib(4);
  vao.AttribBinding(1, 4);
  vao.AttribFormat<vec3>(4, 0);

  TextureCubemap texture;
  texture.CreateStorage(1, GL_RGBA4, face_size);
  const int face_indices[] = {0, 5, 1, 4, 2, 3};
  for (int i = 0; i < 6; ++i) {
    texture.SetSubImage(0, 0, 0, i, face_size, face_size, GL_RGBA,
                        GL_UNSIGNED_SHORT_4_4_4_4, faces[face_indices[i]]);
  }
  glActiveTexture(GL_TEXTURE0);
  texture.Bind();
  program.Uniform("faces", 0);

  program.Use();
  vao.Bind();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto ratio = (float)width / height;
    const auto p = perspective(pi<float>() / 3, ratio, .01f, 100.f);
    program.Uniform("viewPersp", p);

    for (int i = 0; i < grid * grid; ++i) {
      transforms[i] = rotate(transforms[i], 0.02f, rotations[i]);
    }
    transform_buffer.SetSubData(transforms);

    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr,
                            grid * grid);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}
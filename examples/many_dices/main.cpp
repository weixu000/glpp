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
string vertex_shader_source = R"(
#version 450

layout(location = 0) in mat4 model;

out mat4 vModel;

void main() {
  vModel = model;
}
)";

string geometry_shader_source = R"(
#version 450
layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

in mat4 vModel[1];

uniform mat4 viewPersp;

out vec3 vTex;

const vec3 cubeVerts[8] = vec3[8](
  vec3(-0.5 , -0.5, -0.5),  // L B  0
  vec3(-0.5, 0.5, -0.5),    // L T  1
  vec3(0.5, -0.5, -0.5),    // R B  2
  vec3( 0.5, 0.5, -0.5),    // R T  3
  //back face
  vec3(-0.5, -0.5, 0.5),    // L B  4
  vec3(-0.5, 0.5, 0.5),     // L T  5
  vec3(0.5, -0.5, 0.5),     // R B  6
  vec3(0.5, 0.5, 0.5)       // R T  7
);

const int cubeIndices[6][4]  = int[][](
  int[](0,1,2,3), //front
  int[](7,6,3,2), //right
  int[](7,5,6,4),  //back or whatever
  int[](4,0,6,2), //btm
  int[](1,0,5,4), //left
  int[](3,1,7,5)
);

void main() {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
      gl_Position = viewPersp*vModel[0]*vec4(cubeVerts[cubeIndices[i][j]], 1);
      vTex = cubeVerts[cubeIndices[i][j]];
      EmitVertex();
    }
    EndPrimitive();
  }
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
                  GeometryShader{geometry_shader_source},
                  FragmentShader{fragment_shader_source}};

  const auto grid = 7;
  vector<mat4> transforms(grid * grid);
  for (int i = 0; i < grid; ++i) {
    for (int j = 0; j < grid; ++j) {
      transforms[i * grid + j] =
          translate(vec3{(i - (float(grid) - 1) / 2) * 1.8f,
                         (j - (float(grid) - 1) / 2) * 1.8f, -10});
    }
  }

  vector<vec3> rotations(grid * grid);
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
    //    const auto p = ortho(-ratio * 6, ratio * 6, -7.f, 7.f, -2.f, 2.f);
    const auto p = perspective(pi<float>() / 3, ratio, .01f, 100.f);
    program.Uniform("viewPersp", p);

    for (int i = 0; i < grid * grid; ++i) {
      transforms[i] = rotate(transforms[i], 0.02f, rotations[i]);
    }
    vertex_buffer.SetSubData(transforms);

    glDrawArrays(GL_POINTS, 0, grid * grid);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}
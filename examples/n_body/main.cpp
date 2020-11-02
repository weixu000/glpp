#include <array>
#include <common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glpp/glpp.hpp>
#include <string>
#include <vector>

using namespace glm;
using namespace glpp;
using namespace std;

namespace {
struct Particle {
  vec3 position;
  vec3 velocity;
  vec3 acceleration;
};

string computer_shader_source = R"(
#version 450
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle {
  float px, py, pz;
  float vx, vy, vz;
  float ax, ay, az;
};

layout(std430, binding = 0) buffer input_particles {
    Particle inputs[];
};

layout(std430, binding = 1) buffer output_particles {
    Particle outputs[];
};

uniform float dt;

void main() {
  const uint id = gl_WorkGroupID.x;
  const vec3 p = vec3(inputs[id].px, inputs[id].py, inputs[id].pz);
  const vec3 v = vec3(inputs[id].vx, inputs[id].vy, inputs[id].vz);
  const vec3 a = vec3(inputs[id].ax, inputs[id].ay, inputs[id].az);

  vec3 a_o = vec3(0);
  for (uint i = 0; i < inputs.length(); ++i) {
    if (i == id) continue;
    const vec3 d = vec3(inputs[i].px, inputs[i].py, inputs[i].pz)-p;
    a_o += 1/dot(d, d)*normalize(d);
  }

  const vec3 v_o = v+(a+a_o)/2*dt;
  const vec3 p_o = p+(v+v_o)/2*dt;

  outputs[id].px = p_o.x;
  outputs[id].py = p_o.y;
  outputs[id].pz = p_o.z;
  outputs[id].vx = v_o.x;
  outputs[id].vy = v_o.y;
  outputs[id].vz = v_o.z;
  outputs[id].ax = a_o.x;
  outputs[id].ay = a_o.y;
  outputs[id].az = a_o.z;
}
)";

string vertex_shader_source = R"(
#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vel;
layout(location = 2) in vec3 acc;

uniform mat4 view;
uniform mat4 persp;

const float zFar = 1000;
const float accMax = 1;

out vec3 vColor;

float hue2rgb(float f1, float f2, float hue) {
  if (hue < 0.0)
    hue += 1.0;
  else if (hue > 1.0)
    hue -= 1.0;
  float res;
  if ((6.0 * hue) < 1.0)
    res = f1 + (f2 - f1) * 6.0 * hue;
  else if ((2.0 * hue) < 1.0)
    res = f2;
  else if ((3.0 * hue) < 2.0)
    res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
  else
    res = f1;
  return res;
}

vec3 hsl2rgb(vec3 hsl) {
  vec3 rgb;

  if (hsl.y == 0.0) {
    rgb = vec3(hsl.z); // Luminance
  } else {
    float f2;

    if (hsl.z < 0.5)
      f2 = hsl.z * (1.0 + hsl.y);
    else
      f2 = hsl.z + hsl.y - hsl.y * hsl.z;

    float f1 = 2.0 * hsl.z - f2;

    rgb.r = hue2rgb(f1, f2, hsl.x + (1.0/3.0));
    rgb.g = hue2rgb(f1, f2, hsl.x);
    rgb.b = hue2rgb(f1, f2, hsl.x - (1.0/3.0));
  }
  return rgb;
}

void main() {
  const vec4 p_cam = view*vec4(pos, 1);
  gl_Position = persp*p_cam;
  gl_PointSize = max(1, (1-length(p_cam.xyz)/zFar)*20);
  vColor = hsl2rgb(vec3(.60, 1, 0.1+(length(acc)/accMax)/3.14159));
}
)";

string fragment_shader_source = R"(
#version 450

in vec3 vColor;

out vec4 fragColor;

void main()
{
    fragColor = vec4(vColor, 1);
}
)";

vector<Particle> InitParticles() {
  const auto N = 5000;
  vector<Particle> particles(N);
  const auto axis = normalize(vec3{1, 1, 1});
  const auto frame = mat3{orientation(axis, {0, 0, 1})};
  const auto R = 50.f;
  const auto w = axis * sqrt(N / (R * R * R));
  for (int i = 1; i < N; i += 2) {
    const auto r =
        frame * vec3{circularRand(R * pow(linearRand(0.f, 1.f), 1 / 3.f)),
                     gaussRand(0.f, 1.f)};
    particles[i - 1] = Particle{r, cross(w, r), {0, 0, 0}};
    particles[i] = Particle{-r, cross(w, -r), {0, 0, 0}};
  }
  return particles;
}
}  // namespace

int main() {
  const auto window = SetupGL("N-body Simulation");

  Program program{VertexShader{vertex_shader_source},
                  FragmentShader{fragment_shader_source}},
      compute_program{ComputeShader{computer_shader_source}};

  auto particles = InitParticles();

  Buffer buffer[2];
  buffer[0].CreateStorage(particles);
  buffer[1].CreateStorage(particles.size() * sizeof(Particle), nullptr);

  buffer[0].BindBase(BufferTarget::SHADER_STORAGE_BUFFER, 0);
  buffer[1].BindBase(BufferTarget::SHADER_STORAGE_BUFFER, 1);

  VertexArray vao;
  vao.BindVertexBuffer(0, buffer[0], sizeof(Particle));
  vao.EnableAttrib(0, 1, 2);
  vao.AttribBinding(0, 0, 1, 2);
  vao.AttribFormat<vec3>(0, 0);
  vao.AttribFormat<vec3>(1, sizeof(vec3));
  vao.AttribFormat<vec3>(2, 2 * sizeof(vec3));

  vao.Bind();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  auto last_update_t = float(glfwGetTime());

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    compute_program.Use();
    const auto now = float(glfwGetTime());
    compute_program.Uniform("dt", now - last_update_t);
    last_update_t = now;
    glDispatchCompute(particles.size(), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    Buffer::CopySubData(buffer[1], buffer[0], 0, 0,
                        particles.size() * sizeof(Particle));

    program.Use();
    const auto ratio = (float)width / height;
    const auto p = perspective(pi<float>() / 3, ratio, .01f, 1000.f);
    const auto v = lookAt(vec3{0, 0, 500}, vec3{0, 0, 0}, vec3{0, 1, 0});
    program.Uniform("view", v);
    program.Uniform("persp", p);
    glDrawArrays(GL_POINTS, 0, particles.size());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}
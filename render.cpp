// render.cpp
// Defines ray-marching and signed-distance functions
// Based on TinyKaboom, code re-written and cleaned

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include "utils.h"
#include <iostream>
#include <fstream>

using namespace std;

// SDF Constants
// -------------

const float sphere_radius = 1.2;
const float noise_amplitude = 0.2;

// SDF_sphere
// ----------
// Describe sphere in terms of SDF
// Inside of sphere is negative, outside is positive
// Assume sphere is centered at origin

float SDF_sphere(const Vec3f& p) {
  return p.norm() - sphere_radius;
}

// SDF_hedgehog
// ------------
// Generates spikes using interweaving sine functions

float SDF_hedgehog(const Vec3f& p) {
  Vec3f s = Vec3f(p).normalize(sphere_radius);
  float delta = sin(16 * s.x) * sin(16 * s.y) * sin(16 * s.z);
  return p.norm() - (sphere_radius + delta * noise_amplitude);
}

// SDF_normal
// ----------
// See Jamie Wong: Surface Normals and Lighting
// Use gradient to find normal vector to SDF

Vec3f SDF_normal(const Vec3f& pos, float (*SDF)(const Vec3f&)) {
  const float eps = 0.1;
  float d = SDF(pos);
  float normal_x = SDF(pos + Vec3f(eps, 0, 0)) - d;
  float normal_y = SDF(pos + Vec3f(0, eps, 0)) - d;
  float normal_z = SDF(pos + Vec3f(0, 0, eps)) - d;
  return Vec3f(normal_x, normal_y, normal_z).normalize();
}

// calculate_intensity
// -------------------
// Simple BRDF dependant only on distance from normal
// Usage example:
// > float light_intensity = calculate_intensity(light_pos, collision_pos, SDF);
// > pixels[c + r * screen_width] = Vec3f(1, 1, 1) * light_intensity;

float calculate_intensity(const Vec3f& light_pos, const Vec3f& collision_pos, float (*SDF)(const Vec3f&)) {
  Vec3f light_dir = (light_pos - collision_pos).normalize();
  return max(0.4f, light_dir * SDF_normal(collision_pos, SDF));
}

// phong_reflectance
// -----------------
// Implementation of phong reflectance, per Wiki

Vec3f phong_reflection(const Vec3f& diffuse_color,
                       const Vec3f& light_pos,
                       const Vec3f& collision_pos,
                       const Vec3f& camera_pos,
                       float (*SDF)(const Vec3f&))
{
  Vec3f specular_color = Vec3f(1.0, 1.0, 1.0);
  float specular_exponent = 50;

  Vec3f L = (light_pos - collision_pos).normalize();
  Vec3f N = SDF_normal(collision_pos, SDF);
  Vec3f R = (N * dot(L, N) * 2.0) - L;
  Vec3f V = (camera_pos - collision_pos).normalize();

  Vec3f ambient = diffuse_color * 0.1;
  Vec3f diffuse = diffuse_color * dot(L, N);
  Vec3f specular = specular_color * pow(dot(R, V), specular_exponent);
  return ambient + diffuse + specular;
}

// march_ray
// ---------
// TODO: implement += in geometry.h
// Given a ray, performs march operation by iteratively get closer to surface

bool march_ray(const Vec3f& origin, const Vec3f& direction, float (*SDF)(const Vec3f&), Vec3f& position) {
  position = origin;
  for (size_t i = 0; i < 128; i++) {
    float d = SDF(position);
    if (d < 0) return true;
    position = position + direction * max(d * 0.1f, 0.01f);
  }
  return false;
}

// get_direction
// -------------
// Returns direction of ray from camera to pixel (row, col)
// Camera centered in XY-plane, with FOV angle as parameter
// X, Y values are centered at 0.5 offsets of pixel index
// Y multiplied by -1 so zero is at bottom
// Z positioned to conform with FOV constraint

Vec3f get_direction(const size_t row, const size_t col, const float fov, int height, int width) {
  float dir_x = (col + 0.5) - width / 2.0;
  float dir_y = -1.0 * (row + 0.5) + height / 2.0;
  float dir_z = -1.0 * height / (2.0 * tan(fov/2.0));

  return Vec3f(dir_x, dir_y, dir_z).normalize();
}

// generate_image
// --------------
// Writes to PPM file in binary and saves to path

void generate_image(string path, vector<Vec3f>& pixels, int width, int height) {
  ofstream ofs("./image.ppm", ios::binary);
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (size_t pixel = 0; pixel < height * width; pixel++) {
    for (size_t channel = 0; channel < 3; channel++) {
      ofs << (char) clamp((int) (255 * pixels[pixel][channel]), 0, 255);
    }
  }
  ofs.close();
}

// main
// ----
// Loops over pixel values, constructs ray and marches
// Outputs to Portable Pixel Map format
// Parallelize ray marching with OpenMP directives

int main() {
  const int    screen_width   = 640;
  const int    screen_height  = 480;
  const float  fov            = M_PI/3;
  const Vec3f  camera_pos     = Vec3f(0,  0,  3);
  const Vec3f  light_pos      = Vec3f(10, 10, 10);
  float (*SDF) (const Vec3f&) = SDF_hedgehog;
  const Vec3f  diffuse_color  = Vec3f(0.7, 0.2, 0.9);

  vector<Vec3f> pixels(screen_width * screen_height);

#pragma omp parallel for
  for (size_t r = 0; r < screen_height; r++) {
    for (size_t c = 0; c < screen_width; c++) {
      Vec3f direction = get_direction(r, c, fov, screen_height, screen_width);
      Vec3f collision_pos;
      if (march_ray(camera_pos, direction, SDF, collision_pos)) {
        Vec3f illuminated_color = phong_reflection(diffuse_color, light_pos, collision_pos, camera_pos, SDF);
        pixels[c + r * screen_width] = illuminated_color;
      } else {
        pixels[c + r * screen_width] = Vec3f(0.2, 0.7, 0.8);
      }
    }
  }
  generate_image("./image.ppm", pixels, screen_width, screen_height);
  return 0;
}

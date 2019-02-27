// render.cpp
// Defines ray-marching and signed-distance functions
// Based on TinyKaboom, code re-written and cleaned

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include "geometry.h"
#include <iostream>
#include <fstream>

using namespace std;

// Helper Functions
// ----------------

template<class T>
T clamp(const T& v, const T& lo, const T& hi) {
  return max(lo, min(hi, v));
}

// SDF_sphere
// ----------
// Describe sphere in terms of SDF
// Inside of sphere is negative, outside is positive
// Assume sphere is centered at origin

const float sphere_radius = 1.5;

float SDF_sphere(const Vec3f &p) {
  return p.norm() - sphere_radius;
}

// march_ray
// ---------
// TODO: implement += in geometry.h
// Given a ray, performs march operation by iteratively get closer to surface

bool march_ray(const Vec3f &origin, const Vec3f &direction, float (*SDF)(const Vec3f&)) {
  Vec3f position = origin;
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

// main
// ----
// Loops over pixel values, constructs ray and marches
// Outputs to Portable Pixel Map format
// Parallelize ray marching with OpenMP directives

int main() {
  const int   screen_width  = 640;
  const int   screen_height = 480;
  const float fov           = M_PI/3;
  const Vec3f camera_pos    = Vec3f(0, 0, 3);

  vector<Vec3f> pixels(screen_width * screen_height);

#pragma omp parallel for
  for (size_t r = 0; r < screen_height; r++) {
    for (size_t c = 0; c < screen_width; c++) {
      Vec3f direction = get_direction(r, c, fov, screen_height, screen_width);
      pixels[c + r * screen_width] =
        march_ray(camera_pos, direction, SDF_sphere) ? Vec3f(1, 1, 1) : Vec3f(0.2, 0.7, 0.8);
    }
  }

  ofstream ofs("./image.ppm", ios::binary);
  ofs << "P6\n" << screen_width << " " << screen_height << "\n255\n";

  for (size_t pixel = 0; pixel < screen_height * screen_width; pixel++) {
    for (size_t channel = 0; channel < 3; channel++) {
      ofs << (char) clamp((int) (255 * pixels[pixel][channel]), 0, 255);
    }
  }

  ofs.close();

  return 0;
}

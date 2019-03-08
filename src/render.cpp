// render.cpp
// Defines ray-marching and signed-distance functions
// Based on TinyKaboom, code re-written and cleaned

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

// src files
#include "sdf.h"
#include "Vec3.h"
#include "Mat3.h"
#include "threading.h"
#include "utils.h"

using namespace std;

// generate_image
// --------------
// Writes to PPM file in binary and saves to path

void generate_image(string path, vector<Vec3>& pixels, int width, int height) {
  ofstream ofs(path, ios::binary);
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (size_t pixel = 0; pixel < height * width; pixel++) {
    for (size_t channel = 0; channel < 3; channel++) {
      ofs << (char) clamp((int) (255 * pixels[pixel][channel]), 0, 255);
    }
  }
  ofs.close();
}


// SDF_normal
// ----------
// See Jamie Wong: Surface Normals and Lighting
// Use gradient to find normal vector to SDF

Vec3 SDF_normal(const Vec3& pos, double (*SDF)(const Vec3&)) {
  const double eps = 0.1;
  double d = SDF(pos);
  double normal_x = SDF(pos + Vec3(eps, 0, 0)) - d;
  double normal_y = SDF(pos + Vec3(0, eps, 0)) - d;
  double normal_z = SDF(pos + Vec3(0, 0, eps)) - d;
  return Vec3(normal_x, normal_y, normal_z).normalize();
}

// calculate_intensity
// -------------------
// Simple BRDF dependant only on distance from normal
// Usage example:
// > double light_intensity = calculate_intensity(light_pos, collision_pos, SDF);
// > pixels[c + r * screen_width] = Vec3(1, 1, 1) * light_intensity;

double calculate_intensity(const Vec3& light_pos, const Vec3& collision_pos, double (*SDF)(const Vec3&)) {
  Vec3 light_dir = (light_pos - collision_pos).normalize();
  return max(0.4, dot(light_dir, SDF_normal(collision_pos, SDF)));
}

// phong_reflectance
// -----------------
// Implementation of phong reflectance, per Wiki

Vec3 phong_reflection(const Vec3& diffuse_color,
                      const Vec3& light_pos,
                      const Vec3& collision_pos,
                      const Vec3& camera_pos,
                      double (*SDF)(const Vec3&))
{
  Vec3 specular_color = Vec3(1.0, 1.0, 1.0);
  double specular_exponent = 50;

  Vec3 L = (light_pos - collision_pos).normalize();
  Vec3 N = SDF_normal(collision_pos, SDF);
  Vec3 R = (N * dot(L, N) * 2.0) - L;
  Vec3 V = (camera_pos - collision_pos).normalize();

  Vec3 ambient = diffuse_color * 0.1;
  Vec3 diffuse = diffuse_color * dot(L, N);
  Vec3 specular = specular_color * pow(dot(R, V), specular_exponent);
  return ambient + diffuse + specular;
}

// march_ray
// ---------
// Given a ray, performs march operation by iteratively get closer to surface

bool march_ray(const Vec3& origin, const Vec3& direction, double (*SDF)(const Vec3&), Vec3& position) {
  position = origin;
  for (size_t i = 0; i < 128; i++) {
    double d = SDF(position);
    if (d < 0) return true;
    position = position + direction * max(d * 0.1, 0.01);
  }
  return false;
}

// get_direction
// -------------
// Returns direction of ray from camera to pixel (row, col)
// Assumes camera in -z direction, located at origin
// Z-position of picture plane is determined by FOV parameter
// X, Y values are centered at 0.5 offsets of pixel index
// Y multiplied by -1 so zero is at bottom

Vec3 get_direction(const size_t row, 
                   const size_t col,
                   const int height, 
                   const int width, 
                   const double fov)
{
  double dir_x = (col + 0.5) - width / 2.0;
  double dir_y = -1.0 * (row + 0.5) + height / 2.0;
  double dir_z = -1.0 * height / (2.0 * tan(fov/2.0));

  return Vec3(dir_x, dir_y, dir_z).normalize();
}

// camera_matrix
// -------------
// returns matrix which redirects ray to same direction as camera
// only does rotation on XZ-plane so far
// TODO: implement rotation on YZ-plane and compose

const Mat3 camera_matrix(const Vec3& camera_dir) {
  Vec3 y = dot(camera_dir, Vec3(0, 1, 0)) * Vec3(0, 1, 0);
  Vec3 xz = (camera_dir - y).normalize();
  double p_c = dot(xz, Vec3(0, 0, -1));
  double p_s = sqrt(1 - p_c * p_c);
  if (xz[0] > 0) p_s = -p_s; // get angle in anti-clockwise direction
  return Mat3(Vec3(p_c, 0, -p_s), Vec3(0, 1, 0), Vec3(p_s, 0, p_c));
}

// render
// ------
// One rendering, for a given object
// Loops over pixel values, constructs ray and marches
// Outputs to Portable Pixel Map format

void render(string frame_id, const Vec3 camera_pos, const Vec3 camera_dir) {
  cout << "...rendering frame " << frame_id << endl;;

  const int     screen_width  = 640;
  const int     screen_height = 480;

  const Vec3    light_pos     = Vec3(10, 10, 10);
  const Vec3    diffuse_color = Vec3(0.7, 0.2, 0.9);
  double (*SDF) (const Vec3&) = SDF_scene;

  const Mat3    orient_ray    = camera_matrix(camera_dir);
  const double  fov           = M_PI/3;

  vector<Vec3> pixels(screen_width * screen_height);

  for (size_t n = 0; n < screen_height * screen_width; n++) {
    int r = n / screen_width; int c = n % screen_width;

    Vec3 ray_dir = orient_ray * get_direction(r, c, screen_height, screen_width, fov);
    Vec3 collision_pos;
    if (march_ray(camera_pos, ray_dir, SDF, collision_pos)) {
      pixels[c + r * screen_width] = phong_reflection(diffuse_color, light_pos, collision_pos, camera_pos, SDF);
    } else {
      pixels[c + r * screen_width] = Vec3(0.0, 0.0, 0.0);
    }
  }

  string path = "./image" + frame_id + ".ppm";
  generate_image(path, pixels, screen_width, screen_height);
}

// main
// ----
// Generates renderings for animation
// Uses ImageMagick to generate GIFs
// Parallelize ray marching over frames

int main() {
  ThreadPool frame_pool(4);

  cout << "Generating scene..." << endl;;
  for (int n_frame = 0; n_frame <= 20; n_frame++) {

    string frame_id = padded_id(n_frame, /* width = */ 3);

    float c = cos(M_PI * n_frame / 10);
    float s = sin(M_PI * n_frame / 10);

    Vec3 camera_pos =  3.0 * Vec3(s, 0, c);
    Vec3 camera_dir = -1.0 * Vec3(s, 0, c).normalize();

    frame_pool.schedule([frame_id, camera_pos, camera_dir] { 
      render(frame_id, camera_pos, camera_dir); 
    });
  }

  frame_pool.wait();

  cout << endl << "Converting scene to gif..." << endl;
  system("convert -delay 10 -loop 0 image*.ppm scene.gif && rm -rf *.ppm");
  cout << "Done!" << endl;

  return 0;
}

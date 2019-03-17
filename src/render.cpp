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
#include "animate.h"
#include "threading.h"
#include "utils.h"

using namespace std;

// SYSTEM CONSTANTS
// ----------------
// Note: rendering constants defined in render();

const int  NUM_THREADS      = 4;
const int  SCREEN_WIDTH     = 640;
const int  SCREEN_HEIGHT    = 480;
const int  SAMPLE_RATE      = 1;
const int  MARCH_ITERATIONS = 1024;
const bool SHADING          = true;
const int  SHADE_ITERATIONS = 512;

// generate_image
// --------------
// Writes to PPM file in binary and saves to path

void generate_image(string path, vector<Vec3>& pixels, int width, int height) {
  ofstream ofs(path, ios::binary);
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (int pixel = 0; pixel < height * width; pixel++) {
    for (int channel = 0; channel < 3; channel++) {
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
  const double eps = 0.001;
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
// > pixels[c + r * SCREEN_WIDTH] = Vec3(1, 1, 1) * light_intensity;

double calculate_intensity(const Vec3& light_pos, const Vec3& collision_pos, double (*SDF)(const Vec3&)) {
  Vec3 light_dir = (light_pos - collision_pos).normalize();
  return max(0.4, dot(light_dir, SDF_normal(collision_pos, SDF)));
}

// march_ray
// ---------
// Given a ray, performs march operation by iteratively get closer to surface

double march_ray(const Vec3& origin, const Vec3& direction, double (*SDF)(const Vec3&)) {
  double t = 0.001;
  for (int i = 0; i < MARCH_ITERATIONS; i++) {
    double d = SDF(origin + t * direction);
    if (d < 0.0001) return t;
    t += d;
  }
  return 0;
}

// compute_shading
// ---------------
// Computes soft shadows by projecting light onto collision position with SDF
// Start at collision point, try to reach light without intersecting object
// Source: iquilezles.org/www/articles/rmshadows/rmshadows.htm

double compute_shading(const Vec3& light_pos, const Vec3& collision_pos, double (*SDF)(const Vec3&)) {
  int k = 1;
  const Vec3 direction = light_pos - collision_pos;

  double res = 1.0;
  double t = 0.001;

  for (int i = 0; i < SHADE_ITERATIONS; i++) {
    double d = SDF(collision_pos + t * direction);
    if (d < 0.0001) return 0.0;
    res = min(res, k * d / t);
    t += d;
  }

  return res;
}

// phong_reflectance
// -----------------
// Implementation of phong reflectance, per Wikipedia

Vec3 phong_reflection(const Vec3& diffuse_color,
                      double attenuation,
                      const Vec3& light_pos,
                      const Vec3& collision_pos,
                      const Vec3& camera_pos,
                      double (*SDF)(const Vec3&))
{
  Vec3 specular_color = Vec3(1.0, 1.0, 1.0) * attenuation;
  double specular_exponent = 50;

  Vec3 L = (light_pos - collision_pos).normalize();
  Vec3 N = SDF_normal(collision_pos, SDF);
  Vec3 R = (N * dot(L, N) * 2.0) - L;
  Vec3 V = (camera_pos - collision_pos).normalize();

  Vec3 diffuse = attenuation * diffuse_color * clamp(dot(L, N), 0.0, 1.0);
  Vec3 specular = attenuation * specular_color * pow(clamp(dot(R, V), 0.0, 1.0), specular_exponent);
  return diffuse + specular;
}

// get_direction
// -------------
// Returns direction of ray from camera to pixel (row, col)
// Assumes camera in -z direction, located at origin
// Z-position of picture plane is determined by FOV parameter
// X, Y values are centered at 0.5 offsets of pixel index
// Y multiplied by -1 so zero is at bottom

Vec3 get_direction(const int row, const int col, const int width, const int height, const double fov) {
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
// Loops over sample locations, constructs ray and marches
// Supersampling enabled by changing the SAMPLE_RATE constant
// Outputs Portable Pixel Map format and then merged to GIF

void render(string frame_id, const Vec3 camera_pos, const Vec3 camera_dir) {
  cout << "...rendering frame " << frame_id << endl;;

  // RENDERING CONSTANTS
  const vector<Vec3> lights        { Vec3(-2, 1.5, 1.5), Vec3(0, 1.5, 0) };
  /* const vector<Vec3> lights        { Vec3(-2, 0, 0), Vec3(0, 0, 2) }; */
  const Vec3         diffuse_color = Vec3(0.7, 0.2, 0.9);
  double (*SDF)      (const Vec3&) = SDF_scene;
  const Mat3         orient_ray    = camera_matrix(camera_dir);
  const double       fov           = M_PI/3;

  vector<Vec3> pixels(SCREEN_WIDTH * SCREEN_HEIGHT);

  int samples_width = SCREEN_WIDTH * SAMPLE_RATE;
  int samples_height = SCREEN_HEIGHT * SAMPLE_RATE;
  int num_samples = samples_width * samples_height;

  for (int n = 0; n < num_samples; n++) {
    int r = n / samples_width; int c = n % samples_width;

    Vec3 ray_dir = orient_ray * get_direction(r, c, samples_width, samples_height, fov);
    double t = march_ray(camera_pos, ray_dir, SDF);
    Vec3 collision_pos = camera_pos + t * ray_dir;

    Vec3 color = diffuse_color * 0.1;
    if (t > 0) {
      for (Vec3 light_pos : lights) {
        double atten = 1.0 / (1 + 0.1 * (light_pos - collision_pos).norm());
        color += phong_reflection(diffuse_color, atten, light_pos, collision_pos, camera_pos, SDF);
      }
      color /= lights.size();
    }

    double shade = 1.0;
    if (SHADING) {
      for (Vec3 light_pos : lights) {
        shade += compute_shading(light_pos, collision_pos, SDF);
      }
      shade /= lights.size();
      shade = 2 * shade - shade * shade; // 1 - (1 - s)^2
    }
    
    double factor = (1.0 / (SAMPLE_RATE * SAMPLE_RATE));
    pixels[(c / SAMPLE_RATE) + (r / SAMPLE_RATE) * SCREEN_WIDTH] += factor * shade * color;
  }

  string path = "./image" + frame_id + ".ppm";
  generate_image(path, pixels, SCREEN_WIDTH, SCREEN_HEIGHT);
}

// main
// ----
// Generates renderings for animation
// Uses ImageMagick to generate GIFs
// Parallelize ray marching over frames

int main() {
  cout << "Generating scene..." << endl;;
  ThreadPool frame_pool(NUM_THREADS);

  Dolly camera_rig(Vec3(0, 0, 4), Vec3(0, 0, -1));
  /* camera_rig.set_translate(Vec3(0, 0, 3), 5); */
  /* camera_rig.set_rotate(3, -90, 5); */
  /* camera_rig.set_pan(-15, 3); */

  int num_frames = camera_rig.num_moves();
  cout << "Number of frames: " << num_frames << endl;
  for (int n_frame = 0; n_frame < num_frames; n_frame++) {

    string frame_id = padded_id(n_frame, /* width = */ 3);
    frame_t next_frame = camera_rig.get_next_frame();

    frame_pool.schedule([frame_id, next_frame] { 
      render(frame_id, next_frame.pos, next_frame.dir); 
    });
  }

  frame_pool.wait();

  cout << endl << "Converting scene to gif..." << endl;
  system("convert -delay 20 -loop 0 image*.ppm scene.gif && rm -rf *.ppm");
  cout << "Done!" << endl;

  return 0;
}

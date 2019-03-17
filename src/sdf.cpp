#include "Vec3.h"
#include "Mat3.h"
#include "sdf.h"
#include "utils.h"
#include <cmath>
#include <iostream>

using namespace std;

// SDF Composition
// ---------------

inline double SDF_union(double dist_a, double dist_b) {
  return min(dist_a, dist_b);
}

inline double SDF_intersect(double dist_a, double dist_b) {
  return max(dist_a, dist_b);
}

inline double SDF_difference(double dist_a, double dist_b) {
  return max(dist_a, -1.0 * dist_b);
}

// SDF Primitives Functions
// ------------------------

inline double SDF_sphere(const Vec3& p, const double sphere_radius) {
  return p.norm() - sphere_radius;
}

inline double SDF_box(const Vec3& p, const Vec3& s) {
  // s is length of cuboid in each direction
  return vmax(abs(p) - s);
}

inline double SDF_plane(const Vec3& p, const Vec3& c, const Vec3& n) {
  return dot(p - c, n);
}

// SDF Complex Functions
// ---------------------

double SDF_hedgehog(const Vec3& p, const double sphere_radius, const double noise_amplitude) {
  // Generates spikes using interweaving sine functions
  Vec3 s = Vec3(p).normalize(sphere_radius);
  double delta = sin(16 * s.x) * sin(16 * s.y) * sin(16 * s.z);
  return p.norm() - (sphere_radius + delta * noise_amplitude);
}

double SDF_sphere_repeated(const Vec3& p, const double sphere_radius, const double spread) {
  // Repeated spheres using modulus
  Vec3 repeated = Vec3(rmod(p[0], spread), p[1], rmod(p[2], spread));
  return SDF_sphere(repeated - Vec3(spread / 2), sphere_radius);
}

double SDF_cross(const Vec3& p) {
  /* double inf = 1000000.0; */
  double inf = 3.0;
  double box1 = SDF_box(p, Vec3(inf, 1.0, 1.0));
  double box2 = SDF_box(p, Vec3(1.0, inf, 1.0));
  double box3 = SDF_box(p, Vec3(1.0, 1.0, inf));
  return SDF_union(box1, SDF_union(box2, box3));
}

double SDF_wronger(const Vec3& p, double size, int iterations) {
  double d = SDF_box(p, Vec3(size));
  double s = 1.0;

  for (int i = 0; i < iterations; i++) {
    Vec3 a = mod(p * s, size) - (size / 2);
    Vec3 r = Vec3(size) - 3.0 * abs(a);
    s *= 3.0;
    double c = SDF_cross(r) / s;
    d = max(d, c);
  }
  return d;
}

double SDF_menger(const Vec3& p, int iterations) {
  // Per https://aka-san.halcy.de/distance_fields_prefinal.pdf
  // https://iquilezles.org/www/articles/menger/menger.htm

  double d = SDF_box(p, Vec3(1.0));
  double s = 1.0;

  for (int i = 0; i < iterations; i++) {
    Vec3 a = mod(p * s, 2.0) - 1.0;
    Vec3 r = Vec3(1.0) - 3.0 * abs(a);
    s *= 3.0;
    double c = SDF_cross(r) / s;
    d = max(d, c);
  }
  return d;
}

// SDF used when rendering
// -----------------------

double SDF_scene(const Vec3& p) {
  double d1 = SDF_sphere(p, 1);
  double d2 = SDF_plane(p, Vec3(0, -1.5, 0), Vec3(0, 1, 0));
  return SDF_union(d1, d2);
}

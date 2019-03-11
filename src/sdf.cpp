#include "Vec3.h"
#include "sdf.h"
#include "utils.h"
#include <cmath>

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

inline double SDF_cube(const Vec3& p) {
  return vmax(abs(p) - Vec3(1, 1, 1));
}

inline double SDF_plane(const Vec3& p, const Vec3& c, const Vec3& n) {
  return dot(p - c, n);
}

// SDF Complex Functions
// ---------------------

double SDF_hedgehog(const Vec3& p, const double sphere_radius, const double noise_amplitude) {
  Vec3 s = Vec3(p).normalize(sphere_radius);
  double delta = sin(16 * s.x) * sin(16 * s.y) * sin(16 * s.z);
  return p.norm() - (sphere_radius + delta * noise_amplitude);
}

double SDF_sphere_repeated(const Vec3& p, const double sphere_radius, const double spread) {
  Vec3 repeated = Vec3(rmod(p[0], spread), p[1], rmod(p[2], spread));
  return SDF_sphere(repeated - Vec3(spread / 2), sphere_radius);
}


// SDF used when rendering
// -----------------------

double SDF_scene(const Vec3& p) {
  double d1 = SDF_sphere_repeated(p, 0.3, 1.0);
  double d2 = SDF_plane(p, Vec3(0, -2.0, 0), Vec3(0, 1, 0));
  return SDF_union(d1, d2);
}

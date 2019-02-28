#include "utils.h"
#include "sdf.h"
#include <cmath>

using namespace std;

// SDF Constants
// -------------

const double sphere_radius = 1.2;
const double noise_amplitude = 0.2;

// SDF Composition
// ---------------

double SDF_intersect(double dist_a, double dist_b) {
  return max(dist_a, dist_b);
}

double SDF_union(double dist_a, double dist_b) {
  return min(dist_a, dist_b);
}

double SDF_difference(double dist_a, double dist_b) {
  return max(dist_a, -1.0 * dist_b);
}

// SDF Primitives Functions
// ------------------------

double SDF_sphere(const Vec3& p) {
  return p.norm() - sphere_radius;
}

double SDF_cube(const Vec3& p) {
  return vmax(abs(p) - Vec3(0.5, 1, 1));
}

// SDF Complex Functions
// ---------------------

double SDF_hedgehog(const Vec3& p) {
  Vec3 s = Vec3(p).normalize(sphere_radius);
  double delta = sin(16 * s.x) * sin(16 * s.y) * sin(16 * s.z);
  return p.norm() - (sphere_radius + delta * noise_amplitude);
}

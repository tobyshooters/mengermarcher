#ifndef __SDF_H__
#define __SDF_H__
#include "utils.h"

// SDF Composition
// ---------------

double SDF_intersect(double dist_a, double dist_b);
double SDF_union(double dist_a, double dist_b);
double SDF_difference(double dist_a, double dist_b);

// SDF_sphere
// ----------
// Describe sphere in terms of SDF
// Inside of sphere is negative, outside is positive
// Assume sphere is centered at origin

double SDF_sphere(const Vec3& p);
double SDF_cube(const Vec3& p);

// SDF_hedgehog
// ------------
// Generates spikes using interweaving sine functions

double SDF_hedgehog(const Vec3& p);

#endif //__SDF_H__

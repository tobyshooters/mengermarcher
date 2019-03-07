#ifndef __MAT3_H__
#define __MAT3_H__
#include <cmath>
#include "Vec3.h"

class Mat3 {
  public:
    Mat3(void) {}
    Mat3(Vec3 x, Vec3 y, Vec3 z) { entries[0] = x; entries[1] = y; entries[2] = z; }

    double& operator()(int i, int j) { return entries[j][i]; }
    const double& operator()(int i, int j) const { return entries[j][i]; }

    Vec3& operator[](int j) { return entries[j]; }
    const Vec3& operator[](int j) const { return entries[j]; }

    Vec3 operator*(const Vec3& v) const {
      return v[0] * entries[0] + v[1] * entries[1] + v[2] * entries[2];
    }

    Mat3 operator*(const Mat3& m) const {
      return Mat3((*this) * m[0], (*this) * m[1], (*this) * m[2]);
    }

  private:
    Vec3 entries[3];

};

#endif //__MAT3_H__

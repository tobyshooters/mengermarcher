#ifndef __VEC3_H__
#define __VEC3_H__
#include <cmath>
#include <algorithm>

// Vec3 Implementation
// -------------------

class Vec3 {
  public:
    double x, y, z;

    Vec3() : x(0.0), y(0.0), z(0.0) {}
    Vec3(double c) : x(c), y(c), z(c) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}

    inline double& operator[] (const int& index) { return (&x)[index]; }
    inline const double& operator[] (const int& index) const { return (&x)[index]; }

    inline Vec3 operator+ (const Vec3& v) const   { return Vec3(x + v.x,   y + v.y,   z + v.z);   }
    inline Vec3 operator- (const Vec3& v) const   { return Vec3(x - v.x,   y - v.y,   z - v.z);   }
    inline Vec3 operator* (const double& c) const { return Vec3(x * c,     y * c,     z * c);     }
    inline Vec3 operator/ (const double& c) const { return Vec3(x * 1.0/c, y * 1.0/c, z * 1.0/c); }

    inline void operator+= (const Vec3& v)   { x += v.x;   y += v.y;   z += v.z;   }
    inline void operator-= (const Vec3& v)   { x -= v.x;   y -= v.y;   z -= v.z;   }
    inline void operator*= (const double& c) { x *= c;     y *= c;     z *= c;     }
    inline void operator/= (const double& c) { x *= 1.0/c; y *= 1.0/c; z *= 1.0/c; }

    inline double norm(void) const { return sqrt(x*x + y*y + z*z); }
    inline Vec3& normalize(double l=1) { (*this) *= (l / norm()); return *this; }
};

// Vector Functions
// ----------------

inline Vec3 operator* (const double& c, const Vec3& v) {
  return Vec3(c * v.x, c * v.y, c * v.z);
}

inline double dot(const Vec3& u, const Vec3& v) {
  return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline Vec3 abs(const Vec3& v) {
  return Vec3(abs(v.x), abs(v.y), abs(v.z));
}

inline double vmax(const Vec3& v) {
  return std::max(std::max(v.x, v.y), v.z);
}

#endif //__VEC3_H__

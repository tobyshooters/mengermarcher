// Original implementation by github.com/ssloy/tinykaboom
// Expanded by Cristobal Sciutto

#ifndef __UTILS_H__
#define __UTILS_H__
#include <cmath>
#include <cassert>

// Generic Vector Struct Type backed by Array
template <size_t DIM, typename T>
struct vec {
    vec() { for (size_t i=DIM; i--; data_[i] = T()); }
          T& operator[](const size_t i)       { assert(i<DIM); return data_[i]; }
    const T& operator[](const size_t i) const { assert(i<DIM); return data_[i]; }
private:
    T data_[DIM];
};

// Vec3 Implementation done with explicit member variables for components
template <typename T>
struct vec<3,T> {
    vec() : x(T()), y(T()), z(T()) {}
    vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
          T& operator[](const size_t i)       { assert(i<3); return i<=0 ? x : (1==i ? y : z); }
    const T& operator[](const size_t i) const { assert(i<3); return i<=0 ? x : (1==i ? y : z); }
    float norm() const { return std::sqrt(x*x+y*y+z*z); }
    vec<3,T> & normalize(T l=1) { *this = (*this)*(l/norm()); return *this; }
    T x,y,z;
};

// Element-wise Multiplication Operator
template<size_t DIM, typename T>
T operator*(const vec<DIM,T>& lhs, const vec<DIM,T>& rhs) {
    T ret = T();
    for (size_t i=DIM; i--; ret+=lhs[i]*rhs[i]);
    return ret;
}

// Element-wise Addition Operator
template<size_t DIM, typename T>
vec<DIM,T> operator+(vec<DIM,T> lhs, const vec<DIM,T>& rhs) {
    for (size_t i=DIM; i--; lhs[i]+=rhs[i]);
    return lhs;
}

// Element-wise Subtraction Operator
template<size_t DIM, typename T>
vec<DIM,T> operator-(vec<DIM,T> lhs, const vec<DIM,T>& rhs) {
    for (size_t i=DIM; i--; lhs[i]-=rhs[i]);
    return lhs;
}

// Scalar Multiplication Operator
template<size_t DIM, typename T, typename U>
vec<DIM,T> operator*(const vec<DIM,T> &lhs, const U& rhs) {
    vec<DIM,T> ret;
    for (size_t i=DIM; i--; ret[i]=lhs[i]*rhs);
    return ret;
}

// Inverse Operator
template<size_t DIM,typename T>
vec<DIM,T> operator-(const vec<DIM,T> &lhs) {
    return lhs*T(-1);
}

// Basic Vector typedef with Floats
typedef vec<3, float> Vec3f;

// Helper Functions

template<class T>
T clamp(const T& v, const T& lo, const T& hi) {
  return std::max(lo, std::min(hi, v));
}

inline float dot(const Vec3f &u, const Vec3f &v) {
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

#endif //__UTILS_H__
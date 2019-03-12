#ifndef __UTILS_H__
#define __UTILS_H__

#include <algorithm>
#include <iomanip>
#include <sstream>

// Util Functions
// --------------

template<class T>
T clamp(const T& v, const T& lo, const T& hi) {
  return std::max(lo, std::min(hi, v));
}

inline std::string padded_id(int n, int width) {
  std::ostringstream out;
  out << std::setfill('0') << std::setw(width) << n;
  return out.str();
}

// Brings negative numbers into positive range
inline double rmod(double v, double m) {
  double mod = fmod(v, m);
  return v > 0 ? mod : mod + m;
}

#endif //__UTILS_H__

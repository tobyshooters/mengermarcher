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

std::string padded_id(int n, int width) {
  std::ostringstream out;
  out << std::setfill('0') << std::setw(width) << n;
  return out.str();
}

#endif //__UTILS_H__

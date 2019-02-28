#ifndef __UTILS_H__
#define __UTILS_H__
#include <algorithm>

// Util Functions
// --------------

template<class T>
T clamp(const T& v, const T& lo, const T& hi) {
  return std::max(lo, std::min(hi, v));
}

#endif //__UTILS_H__

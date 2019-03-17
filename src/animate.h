#ifndef __ANIMATE_H__
#define __ANIMATE_H__

#define _USE_MATH_DEFINES
#include "Vec3.h"
#include "Mat3.h"
#include <cmath>
#include <queue>

using namespace std;

Mat3 get_rotation_matrix(double degrees) {
  double radians = (degrees * M_PI) / 180;
  double p_c = cos(radians);
  double p_s = sin(radians);
  return Mat3(Vec3(p_c, 0, -p_s), Vec3(0, 1, 0), Vec3(p_s, 0, p_c));
}

typedef struct ray {
  Vec3 pos;
  Vec3 dir;
} frame_t;

class Dolly {
  public:
    Dolly(Vec3 pos, Vec3 dir) {
      frame_t init = { .pos = pos, .dir = dir };
      curr = init;
      frames.push(init);
    }

    frame_t get_next_frame() {
      frame_t next = frames.front();
      frames.pop();
      return next;
    }

    int num_moves() {
      return frames.size();
    }

    void set_translate(Vec3 dest, double steps) {
      Vec3 delta = (1.0 / steps) * (dest - curr.pos);
      for (int i = 0; i < steps; i++) {
        curr.pos += delta;
        frames.push(curr);
      }
    }

    // + is counterclockwise
    void set_pan(double degrees, double steps) {
      Mat3 rot =  get_rotation_matrix(degrees / steps);
      for (int i = 0; i < steps; i++) {
        curr.dir = rot * curr.dir;
        frames.push(curr);
      }
    }

    void set_rotate(double radius, double degrees, double steps) {
      Mat3 rot =  get_rotation_matrix(degrees / steps);
      Vec3 center = curr.pos + radius * curr.dir;
      for (int i = 0; i < steps; i++) {
        curr.pos = center + rot * (curr.pos - center);
        curr.dir = (center - curr.pos).normalize();
        frames.push(curr);
      }
    }

  private:
    queue<frame_t> frames;
    frame_t curr;
};

#endif //__ANIMATE_H__

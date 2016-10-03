#ifndef MEASUREMENT_H
#define MEASUREMENT_H

struct Measurement {
  Measurement() : x(0), y(0), z(0), ax(0), ay(0), az(0) { };
  int x, y, z, ax, ay, az;

  bool operator==(Measurement const& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z)
      && (ax == rhs.ax) && (ay == rhs.ay) && (az == rhs.az);
  }
};

#endif

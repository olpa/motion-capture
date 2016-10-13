#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <iostream>

struct Measurement {
public:
  Measurement();
  bool operator==(Measurement const& rhs) const;
  void cast_to_signed();

  int16_t x, y, z, ax, ay, az, temperature;
};

#endif

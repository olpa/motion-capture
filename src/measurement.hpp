#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <iostream>

struct Measurement {
public:
  Measurement();
  bool operator==(Measurement const& rhs) const;

  int x, y, z, ax, ay, az, temperature;
};

#endif

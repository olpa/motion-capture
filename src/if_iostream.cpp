#include "if_iostream.hpp"

Measurement measure(std::istream& is) {
  Measurement m;
  int i1, i2, i3, i4, i5, i6, i7;
  is >> std::hex >> i1 >> i2 >> i3 >> i4 >> i5 >> i6 >> i7 >> std::dec;
  m.x = i1;
  m.y = i2;
  m.z = i3;
  m.ax = i4;
  m.ay = i5;
  m.az = i6;
  m.temperature = i7;
  return m;
}

std::ostream& operator<<(std::ostream& os, Measurement const& m) {
  os << std::hex << m.x << ' ' << m.y << ' ' << m.z << ' ' << m.ax << ' ' << m.ay << ' ' << m.az << ' ' << m.temperature << std::dec;
  return os;
}

#include "if_iostream.hpp"

Measurement measure(std::istream& is) {
  Measurement m;
  is >> std::hex >> m.x >> m.y >> m.z >> m.ax >> m.ay >> m.az >> m.temperature >> std::dec;
  return m;
}

std::ostream& operator<<(std::ostream& os, Measurement const& m) {
  os << std::hex << m.x << ' ' << m.y << ' ' << m.z << ' ' << m.ax << ' ' << m.ay << ' ' << m.az << ' ' << m.temperature << std::dec;
}

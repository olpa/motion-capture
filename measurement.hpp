#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <cstdint>
#include <iostream>
#include <array>

class BigEndian16 {
  uint16_t val;
public:
  operator uint16_t() const;
  friend std::ostream& operator<<(std::ostream& os, BigEndian16 const& obj);
  friend std::istream& operator>>(std::istream& is, BigEndian16& obj);
};

class Measurement {
  constexpr static int first_reg = 0x3b; // 59 dec
  constexpr static int last_reg  = 0x48; // 72 dec
  constexpr static int n_words   = (last_reg - first_reg + 1)/2;
  using bytedata_t = std::array<int8_t, last_reg - first_reg + 1>;
  using worddata_t = std::array<BigEndian16, n_words>;
  union {
    bytedata_t bytes;
    worddata_t data;
  };
public:
  void measure(int fd);
  friend std::ostream& operator<<(std::ostream& os, Measurement const& obj);
  friend std::istream& operator>>(std::istream& is, Measurement& obj);
};

#endif

#include <sstream>
#include "measurement.hpp"
#include "if_iostream.hpp"
#include <gmock/gmock.h>
using namespace testing;

TEST(Iostream, ReadMeasurement) {
  std::stringstream ss("204c 64 e4 f350 4b9 ff70 feb1 ");

  Measurement m = measure(ss);

  ASSERT_THAT(m.x, Eq(0x204c));
  ASSERT_THAT(m.y, Eq(0x64));
  ASSERT_THAT(m.z, Eq(0xe4));
  ASSERT_THAT(m.ax, Eq(0xf350-0xffff-1));
  ASSERT_THAT(m.ay, Eq(0x4b9));
  ASSERT_THAT(m.az, Eq(0xff70-0xffff-1));
  ASSERT_THAT(m.temperature, Eq(0xfeb1-0xffff-1));
}

TEST(Iostream, WriteMeasurement) {
  Measurement m;
  m.x = 1;
  m.y = 2;
  m.z = 4;
  m.ax = 8;
  m.ay = 16;
  m.az = 32;
  m.temperature = 64;
  std::stringstream ss;

  ss << m;

  ASSERT_THAT(ss.str(), Eq("1 2 4 8 10 20 40"));
}

TEST(Iostream, WriteNegativeValues) {
  Measurement m;
  m.x = -1;
  m.y = -2;
  m.z = -4;
  m.ax = -8;
  m.ay = -16;
  m.az = -32;
  m.temperature = -64;
  std::stringstream ss;

  ss << m;

  ASSERT_THAT(ss.str(), Eq("ffff fffe fffc fff8 fff0 ffe0 ffc0"));
}

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

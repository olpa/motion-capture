#include "measurement.hpp"
#include <gmock/gmock.h>

using namespace testing;

TEST(Measurement, Instantiates) {
  Measurement m;
}

TEST(Measurement, InstantiatesEmpty) {
  Measurement m;

  void();

  ASSERT_THAT(m.x, Eq(0));
  ASSERT_THAT(m.y, Eq(0));
  ASSERT_THAT(m.z, Eq(0));
  ASSERT_THAT(m.ax, Eq(0));
  ASSERT_THAT(m.ay, Eq(0));
  ASSERT_THAT(m.az, Eq(0));
  ASSERT_THAT(m.temperature, Eq(0));
}

TEST(Measurement, EqualCompare) {
  Measurement m1, m2;
  m1.x = 7;
  m2.x = 7;

  void();

  ASSERT_THAT(m1, Eq(m2));
}

TEST(Measurement, UnsignedBecomesSigned) {
  Measurement m;
  m.x = 0xffff;

  void();

  ASSERT_THAT(m.x, Eq(-1));
}

int main(int argc, char**argv) {
  InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

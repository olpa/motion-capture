#include "measurement.hpp"
#include <gmock/gmock.h>

using namespace testing;

TEST(Measurement, Instaltiates) {
  Measurement m;
}

TEST(Measurement, CanCompare) {
  Measurement m1, m2;
  ASSERT_THAT(m1, Eq(m2));
  // Not eq
}

int main(int argc, char**argv) {
  InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

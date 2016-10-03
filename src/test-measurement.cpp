#include <gmock/gmock.h>

using namespace testing;

struct Measurement {
  Measurement() : x(0), y(0), z(0), ax(0), ay(0), az(0) { };
  int x, y, z, ax, ay, az;
};

TEST(Measurement, Instantiates) {
  Measurement m;
}

TEST(Measurement, InstantiatesEmpty) {
  Measurement m;
  ASSERT_THAT(m.x, Eq(0));
  ASSERT_THAT(m.y, Eq(0));
  ASSERT_THAT(m.z, Eq(0));
  ASSERT_THAT(m.ax, Eq(0));
  ASSERT_THAT(m.ay, Eq(0));
  ASSERT_THAT(m.az, Eq(0));
}

//TEST(Measurement, CanEqualCompare) {
//  Measurement m1, m2;
//  ASSERT_THAT(m1, Eq(m2));
//  // Not eq
//}

int main(int argc, char**argv) {
  InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

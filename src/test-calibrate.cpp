#include "measurement.hpp"
#include <gmock/gmock.h>

class Calibrate {
public:
  Measurement get_zero_point() const {
    return Measurement();
  }
};

using namespace testing;

TEST(Calibrate, Instaltiates) {
  Calibrate c;
  (void)c;
}

TEST(Calibrate, DefaultReturnsZeroValues) {
  Calibrate c;
  Measurement mes = c.get_zero_point();
  ASSERT_THAT(mes, Eq(Measurement()));
}

int main(int argc, char**argv) {
  InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

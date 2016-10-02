#include <gmock/gmock.h>

class Calibrate {
};

using namespace testing;

TEST(Calibrate, Instaltiates) {
  Calibrate c;
}

int main(int argc, char**argv) {
  InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

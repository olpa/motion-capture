#include <fstream>
#include <iostream>
#include <ios>
#include <iterator>
#include <system_error>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "measurement.hpp"

const std::string icsfile("/dev/i2c-1");
constexpr int mpuaddr{0x68};
constexpr int reg{67};

int main(int args, char** argv) {
  std::fstream fdev;
  int hdev = ::open("/dev/null", 0);
  if (-1 == hdev) {
    std::cerr << "Checking /dev/null: " << ::strerror(errno) << std::endl;
    return 1;
  }
  assert(! ::close(hdev)); // The handle will be reused
  fdev.exceptions(std::ios::failbit | std::ios::badbit);
  try {
    fdev.open(icsfile);
  } catch (std::ios_base::failure &e) {
    std::cerr << icsfile << ": " << ::strerror(errno) << std::endl;
    return 1;
  }
  if (::ioctl (hdev, I2C_SLAVE, mpuaddr) < 0) {
    std::cerr << "ioctl failed: " << ::strerror(errno) << std::endl;
    return 1;
  }
  Measurement m;
  for (int i = 0; i < 10; i++) {
    m.measure(hdev);
    std::cout << m << std::endl;
    ::sleep(1);
  }
  return 0;
}

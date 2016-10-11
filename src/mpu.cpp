#include <iostream>
#include <system_error>
#include <string>
#include <unistd.h>

#include "measurement.hpp"
#include "if_mpu6050.hpp"
#include "if_iostream.hpp"

const std::string i2c_device_file("/dev/i2c-1");
constexpr int mpu_address{0x68};

int main(int args, char** argv) {
  try {
    i2c_Device_File hdev(i2c_device_file, mpu_address);
    Measurement m;
    for (int i = 0; i < 10; i++) {
      m = measure(hdev);
      std::cout << m << std::endl;
      ::sleep(1);
    }
  } catch (std::system_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }
  return 0;
}

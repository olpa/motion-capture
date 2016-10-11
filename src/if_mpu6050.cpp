#include <string>
#include <system_error>
#include <fcntl.h>
#include <unistd.h>
#include <cstdint>
#include <arpa/inet.h>
#include "if_mpu6050.hpp"
#include "i2c_lib.hpp"

i2c_Device_File::i2c_Device_File(std::string const& file_name, int mpu_address)
: handle(::open(file_name.c_str(), O_RDWR))
{
  if (-1 == handle) {
    throw std::system_error(errno, std::system_category());
  }
  if (i2c_set_slave_address(handle, mpu_address) < 0) {
    throw std::system_error(errno, std::system_category());
  }
}

i2c_Device_File::~i2c_Device_File() {
  if (-1 != handle) {
    ::close(handle);
    handle = -1;
  }
}

i2c_Device_File::i2c_Device_File(i2c_Device_File&& arg) {
  handle = arg.handle;
  arg.handle = -1;
}

i2c_Device_File& i2c_Device_File::operator=(i2c_Device_File&& rhs) {
  if (-1 != handle) {
    ::close(handle);
  }
  handle = rhs.handle;
  rhs.handle = -1;
  return *this;
}

int i2c_Device_File::get_handle() const {
  return handle;
}

Measurement measure(i2c_Device_File& device) {
  Measurement m;
  int h = device.get_handle();
  m.x = ntohs(i2c_smbus_read_word_data(h, 0x3b));
  m.y = ntohs(i2c_smbus_read_word_data(h, 0x3d));
  m.z = ntohs(i2c_smbus_read_word_data(h, 0x3f));
  m.ax = ntohs(i2c_smbus_read_word_data(h, 0x41));
  m.ay = ntohs(i2c_smbus_read_word_data(h, 0x43));
  m.az = ntohs(i2c_smbus_read_word_data(h, 0x45));
  m.temperature = ntohs(i2c_smbus_read_word_data(h, 0x47));
  return m;
}

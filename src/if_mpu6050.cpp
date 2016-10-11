#include <string>
#include <system_error>
#include <fcntl.h>
#include <unistd.h>
#include <cstdint>
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

/*
uint16_t read_uint16(i2c_Device_File const& device, int regno) {
  uint16_t network_order_word;
  uint8_t* p = reinterpret_cast<uint8_t*>(&network_order_word);
  int b = i2c_smbus_read_byte_data(device.get_handle(), regno++);
  if (b < 0) {
    throw std::system_error(errno, std::system_category());
  }
  *p++ = b;
  b = i2c_smbus_read_byte_data(device.get_handle(), regno);
  if (b < 0) {
    throw std::system_error(errno, std::system_category());
  }
  *p = b;
  return ntons(network_order_word);
}
*/

Measurement measure(i2c_Device_File& device) {
  Measurement m;
  int h = device.get_handle();
  m.x = i2c_smbus_read_word_data(h, 0x3b);
  m.y = i2c_smbus_read_word_data(h, 0x3d);
  m.z = i2c_smbus_read_word_data(h, 0x3f);
  m.ax = i2c_smbus_read_word_data(h, 0x41);
  m.ay = i2c_smbus_read_word_data(h, 0x43);
  m.az = i2c_smbus_read_word_data(h, 0x45);
  m.temperature = i2c_smbus_read_word_data(h, 0x47);
  return m;
}

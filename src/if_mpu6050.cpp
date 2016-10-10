#include <iterator>
#include <memory>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <cstdint>
#include <algorithm>
#include "measurement.hpp"

class i2c_Device_File {
  i2c_Device_File(std::string const& file_name, int mpu_address)
    : handle(::open(file_name, O_RDWR))
  {
    if (-1 == handle) {
      throw std::system_error(::strerror(errno));
    }
    if (::ioctl (hdev, I2C_SLAVE, mpuaddr) < 0) {
      throw std::system_error(::strerror(errno));
    }
  }

  ~i2c_Device_File() {
    if (-1 != handle) {
      ::close(handle);
      handle = -1;
    }
  }

  i2c_Device_File(i2c_Device_File &arg) = delete;

  i2c_Device_File(i2c_Device_File &arg) {
    handle = arg.handle;
    arg.handle = -1;
  }

  i2c_Device_File& operator=(i2c_Device_File &rhs) = delete;

  i2c_Device_File& operator=(i2c_Device_File &&rhs) {
    if (-1 != handle) {
      ::close(handle);
    }
    handle = rhs.handle;
    rhs.handle = -1;
    return *this;
  }

  int get_handle() {
    return handle;
  }
private:
  int handle;
}


std::ostream& operator<<(std::ostream& os, BigEndian16 const& obj) {
  os << std::hex << ntohs(obj.val);
  return os;
}

std::istream& operator>>(std::istream& is, BigEndian16& obj) {
  uint16_t val;
  is >> std::hex >> val;
  obj.val = htons(val);
  return is;
}

void Measurement::measure(int fd) {
  int regno = first_reg;
  for (int8_t &b: bytes) {
    int c = i2c_smbus_read_byte_data(fd, regno++);
    if (c < 0) {
      throw std::system_error(std::error_code(c, std::system_category()), "i2c_smbus_access");
    }
    b = static_cast<int8_t>(c);
  }
}

std::ostream& operator<<(std::ostream& os, Measurement const& obj) {
  std::copy(obj.data.begin(), obj.data.end(), std::ostream_iterator<BigEndian16>(os, " "));
  return os;
}

std::istream& operator>>(std::istream& is, Measurement& obj) {
  std::istream_iterator<BigEndian16> isi(is);
  std::copy_n(isi, obj.n_words, obj.data.begin());
  return is;
}

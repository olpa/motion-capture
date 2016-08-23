#include <iterator>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <cstdint>
#include <algorithm>
#include "measurement.hpp"

/* Copy-paste from i2ctools */
__s32 i2c_smbus_access(int file, char read_write, __u8 command,
    int size, union i2c_smbus_data *data)
{
  struct i2c_smbus_ioctl_data args;
  __s32 err;

  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;

  err = ioctl(file, I2C_SMBUS, &args);
  if (err == -1)
    err = -errno;
  return err;
}
__s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
  union i2c_smbus_data data;
  int err;

  err = i2c_smbus_access(file, I2C_SMBUS_READ, command,
      I2C_SMBUS_BYTE_DATA, &data);
  if (err < 0)
    return err;

  return 0x0FF & data.byte;
}
/* End of copy-paste */

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

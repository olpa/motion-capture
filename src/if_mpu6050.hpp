#ifndef IF_MPU6050_H
#define IF_MPU6050_H

#include <string>
#include "measurement.hpp"

class i2c_Device_File {
public:
  i2c_Device_File(std::string const& file_name, int mpu_address);
  ~i2c_Device_File();
  i2c_Device_File(i2c_Device_File& arg) = delete;
  i2c_Device_File(i2c_Device_File&& arg);
  i2c_Device_File& operator=(i2c_Device_File& rhs) = delete;
  i2c_Device_File& operator=(i2c_Device_File&& rhs);

  int get_handle() const;

private:
  int handle;
};

Measurement measure(i2c_Device_File& device);

#endif

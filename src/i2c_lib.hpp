#ifndef I2C_LIB_H
#define I2C_LIB_H
#include <cstdint>

int i2c_set_slave_address(int handle, int addr);
int32_t i2c_smbus_read_byte_data(int file, uint8_t command);
int32_t i2c_smbus_read_word_data(int file, uint8_t command);

#endif

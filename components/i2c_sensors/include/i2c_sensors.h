#ifndef __I2C_SENSORS__
#define __I2C_SENSORS__

#include <stdio.h>
#include <stdint.h>
#include <zephyr/device.h>

/**
 * @brief Write I2C bytes
 * 
 * @param i2c_dev I2C peripheral to use
 * @param addr Address to write
 * @param data Data to write
 * @param num_bytes Number of bytes to write
 * @param devaddr7 Addres of the device to write
 * @return int Error code
 */
int i2c_write_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes, uint8_t devaddr7);

/**
 * @brief Read I2C bytes
 * 
 * @param i2c_dev I2C peripheral to use
 * @param addr Address to read
 * @param data Buffer to store read data
 * @param num_bytes Number of bytes to read
 * @param devaddr7 Address of the device to read
 * @return int 
 */
int i2c_read_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes, uint8_t devaddr7);

#endif // __I2C_SENSORS__

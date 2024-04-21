#include "i2c_sensors.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>

#include "device_config.h"

#define I2C_MSG_CONTIGOUS_WRITE_MAX_BYTES	64

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
int i2c_write_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes, uint8_t devaddr7)
{
	struct i2c_msg msgs[2];

	uint8_t buf[64];
	buf[0] = addr;
	memcpy (&buf[1], data, num_bytes);

	/* Send the buffer to write to */
	msgs[0].buf = buf;
	msgs[0].len = num_bytes+1;
	msgs[0].flags = I2C_MSG_WRITE;

	return i2c_transfer(i2c_dev, &msgs[0], 1, devaddr7);
}

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
int i2c_read_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes, uint8_t devaddr7)
{
	struct i2c_msg msgs[2];

	/* Setup I2C messages */

	/* Send the address to read from */
	msgs[0].buf = &addr;
	msgs[0].len = 1U;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Read from device. STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, devaddr7);
}
#ifndef __ACCEL__
#define __ACCEL__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#include "app_types.h"
#include "i2c_sensors.h"
#include "device_config.h"
#include "config_commands.h"
#include "app_gpio.h"

#define ACCEL_I2C_ADDR (0x19)

#define ACCEL_ERROR -1
#define ACCEL_SUCCESS 0

/**
 * @brief Accelerometer data
 * 
 */
typedef struct 
{
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
} accel_data_t;

/**
 * @brief Who am I for the accelerometer - Used as an Identification for the chip
 * 
 * @return int Read value if reading is succesful, error code otherwise
 */
int app_accel_whoami();

/**
 * @brief Set Accelerometer Configuration
 * 
 * @return int error code
 */
int app_accel_config ();

/**
 * @brief Accelerometer trigger enable function
 * 
 */
void accel_trigger_enable(void);

/**
 * @brief Read accelerometer current value
 * 
 * @param accel_data Buffer to store current accelerometer data
 * @return int error code
 */
int app_accel_read(accel_data_t *accel_data);

/**
 * @brief Accelerometer configuration for the FIFO buffer
 * 
 * @return int error code
 */
int app_accel_config_use_fifo_buffer();

void test_read();
void accel_int2_interrupt(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
/**
 * @brief Accelerometer configuration for the FIFO buffer
 * 
 * @return int error code
 */
int app_accel_config_not_use_fifo_buffer();

#endif // __ACCEL__

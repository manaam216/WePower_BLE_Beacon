#ifndef __ACCEL__
#define __ACCEL__

#include <stdint.h>

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

#if (USE_ZEPHYR_SENSOR)
/**
 * @brief Read Accelerometer data using Zephyr APIs
 * 
 * @param accel_data Buffer to store read data
 * @return int Error code
 */
int app_accel_service(accel_data_t *accel_data);
#endif

#endif // __ACCEL__

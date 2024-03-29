#ifndef __TEMP_PRESSURE__
#define __TEMP_PRESSURE__

#include <stdint.h>

#define TEMP_PRESSURE_ERROR 		-1
#define TEMP_PRESSURE_SUCCESS 		 0

/**
 * @brief Temperature and Pressure sensor data
 * 
 */
typedef struct 
{
    int16_t temp;
    int16_t pressure;
} temp_pressure_data_t;

//int app_temp_pressure_service(temp_pressure_data_t *temp_pressure_data);

/**
 * @brief Enable trigger of pressure and temperature sensor
 * 
 * @return int error code
 */
int app_temp_pressure_trigger(void);

/**
 * @brief Read temperature and pressure sensor
 * 
 * @param temp_pressure_data Buffer to store temperature and pressure data
 * @return int error code
 */
int app_temp_pressure_read(temp_pressure_data_t *temp_pressure_data);

/**
 * @brief Configure temperature and pressure sensor to enable interrupt on DRDY pin
 * 
 * @return int error code
 */
int enable_temp_pressure_sensor_interrupt_config(void);


#endif // __TEMP_PRESSURE__

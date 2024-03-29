#include "temp_pressure.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/types.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/drivers/i2c.h>
#include <nrfx_gpiote.h>

#include "app_types.h"
#include "device_config.h"
#include "config_commands.h"
#include "i2c_sensors.h"
#include "app_gpio.h"

LOG_MODULE_DECLARE(wepower);

#define TEMP_PRESSURE_SENSOR_I2C_ADDR (0x5C)

#define TEMP_PRESSURE_SENSOR_CNTRL_REG1_ADDR	0x10
#define TEMP_PRESSURE_SENSOR_CNTRL_REG2_ADDR	0x11
#define TEMP_PRESSURE_SENSOR_CNTRL_REG3_ADDR	0x12

#define ENABLE_DRDY_INTERRUPT_COMMAND			0x04
#define ENABLE_ONE_SHOT_AND_ADDR_INC_COMMAND	0x11

#define TEMP_PRESSURE_SENSOR_DATA_READY_TIMEOUT		100
#define TEMP_PRESSURE_SENSOR_READING_BUFFER_SIZE 	5

#define TEMP_PRESSURE_SENSOR_READ_REG_START_ADDR	0x28

const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

/**
 * @brief Configure temperature and pressure sensor to enable interrupt on DRDY pin
 * 
 * @return int error code
 */
int enable_temp_pressure_sensor_interrupt_config(void)
{
	uint8_t config = ENABLE_DRDY_INTERRUPT_COMMAND;

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Temperatue and pressure sensor interrupt config failed - I2C device is not ready");
		return TEMP_PRESSURE_ERROR;
	}

	return i2c_write_bytes(i2c_dev, TEMP_PRESSURE_SENSOR_CNTRL_REG3_ADDR, &config, sizeof(config), TEMP_PRESSURE_SENSOR_I2C_ADDR);
}

/**
 * @brief Enable trigger of pressure and temperature sensor
 * 
 * @return int error code
 */
int app_temp_pressure_trigger(void)
{
	uint8_t trigger = ENABLE_ONE_SHOT_AND_ADDR_INC_COMMAND; 

	if (!device_is_ready(i2c_dev)) {
		LOG_ERR("Temperature and Pressure sensor trigger config failed - I2C device not ready");
		return TEMP_PRESSURE_ERROR;
	}

	return i2c_write_bytes(i2c_dev, TEMP_PRESSURE_SENSOR_CNTRL_REG2_ADDR, &trigger, sizeof(trigger), TEMP_PRESSURE_SENSOR_I2C_ADDR);
}


/**
 * @brief Read temperature and pressure sensor
 * 
 * @param temp_pressure_data Buffer to store temperature and pressure data
 * @return int error code
 */
int app_temp_pressure_read(temp_pressure_data_t *temp_pressure_data)
{
	uint8_t buffer_to_read_data[TEMP_PRESSURE_SENSOR_READING_BUFFER_SIZE];
	uint16_t timeout = TEMP_PRESSURE_SENSOR_DATA_READY_TIMEOUT;
	while (!get_tps_drdy_pin_status() && timeout--)
	{
		k_usleep (100);
	}

	if (timeout == 0)
	{	
		LOG_ERR("Temperature and Pressure sensor DRDY timeout");
		return TEMP_PRESSURE_ERROR;
	}

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Temperature and Pressure sensor reading failed - I2C device not ready");
		return TEMP_PRESSURE_ERROR;
	}

	// Reseting the global buffers 
	temp_pressure_data->pressure = 0x8000;
	temp_pressure_data->temp = 0x8000;
	
	int ret = i2c_read_bytes(i2c_dev, TEMP_PRESSURE_SENSOR_READ_REG_START_ADDR, buffer_to_read_data, TEMP_PRESSURE_SENSOR_READING_BUFFER_SIZE, TEMP_PRESSURE_SENSOR_I2C_ADDR);
    if (ret) 
    {
		LOG_ERR("Temprature and Pressure sensor reading failed - I2C reading failed");
		return TEMP_PRESSURE_ERROR;
    } 
    else 
    {
		i32_u8_t pressure;
		pressure.u8[2] = buffer_to_read_data[2];
		pressure.u8[1] = buffer_to_read_data[1];
		pressure.u8[0] = buffer_to_read_data[0];

		/**
		 * @brief 1 KPa = 10 hPa . The datasheet of TPS sensor says to divide the value (pressure.i32) by 4096 
		 * to obtain the value in hPa. Our App shows the value in KPa so multiplying by 10 is necessary.
		 * 
		 */
        temp_pressure_data->pressure = (pressure.i32 *10)/4096;
        temp_pressure_data->temp = (int16_t)((((uint16_t)buffer_to_read_data[3]) + (((uint16_t)buffer_to_read_data[4])<<8)));

		LOG_INF("IMU: 0x%02X%02X%02X 0x%02X%02X", 
				buffer_to_read_data[2], 
				buffer_to_read_data[1], 
				buffer_to_read_data[0], 
				buffer_to_read_data[4], 
				buffer_to_read_data[3]);
		LOG_INF( "IMU: %d kPa; %d C", temp_pressure_data->pressure, temp_pressure_data->temp);

        return TEMP_PRESSURE_SUCCESS;
   }
}

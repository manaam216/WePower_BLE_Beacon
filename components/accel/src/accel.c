#include "accel.h"
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

#include "nrfx_gpiote.h"

LOG_MODULE_DECLARE(wepower);

#define REGISTER_VALUE_WHO_AM_I	0xFF
#define WHO_AM_I_NO_OF_BYTES	1

#define ACC_CONFIG_MSG_LEN 			4
#define ACC_FIFO_CONFIG_MSG_LEN     1
#define ACC_CONFIG_REGISTER_CNTRL1_ADDR	0x20
#define ACC_CONFIG_REGISTER_CNTRL2_ADDR	0x22
#define ACC_CONFIG_REGISTER_FIFO_CNTRL_ADDR 0x2E
#define ACC_DRDY_READY_TIMEOUT		100
#define ACC_READ_DATA_BUFFER_SIZE	6
#define ACC_READ_DATA_REG_ADDR		0x28

struct k_work accelerometer_fifo_evt_work_item;

accel_data_t temp_buffer_for_fifo[32];
accel_data_t temp_buf;

void test_read()
{
	for (uint8_t i = 0; i < 32; i++)
	{
		app_accel_read(&temp_buf);
		memcpy((&temp_buffer_for_fifo)+i, &temp_buf,sizeof(temp_buf));
	}
	LOG_HEXDUMP_INF(temp_buffer_for_fifo, sizeof(temp_buffer_for_fifo),"ACCEL Data");
}

/**
 * @brief Function used to start the advertising on Bluetooth
 * 
 * @param work Work Item from work queue
 */
void accelerometer_events_work_handler(struct k_work *work)
{
	
	disable_accel_int2_interrupts();
test_read();
	enable_accel_int2_interrupts();
}

void accel_int2_interrupt(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("\r AYAA \n");
	k_work_submit(&accelerometer_fifo_evt_work_item);
}

/**
 * @brief Who am I for the accelerometer - Used as an Identification for the chip
 * 
 * @return int Read value if reading is succesful, error code otherwise
 */
int app_accel_whoami()
{
	uint8_t whoami = 0;
	int ret = 0;
    
	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev))
	{
		LOG_ERR("Failed to read WhoAMI value - I2C error");	
		ret =  ACCEL_ERROR;
	}
	else
	{
		ret = i2c_read_bytes(i2c_dev, REGISTER_VALUE_WHO_AM_I, &whoami, WHO_AM_I_NO_OF_BYTES, ACCEL_I2C_ADDR);
		if (ret == ACCEL_SUCCESS) 
		{
			ret =  whoami; // 0x44 will not sign extend
		}
	}
	
	return ret;
}

/**
 * @brief Accelerometer configuration for the FIFO buffer
 * 
 * @return int error code
 */
int app_accel_config_use_fifo_buffer()
{
	uint8_t config;
	int ret = 0;

	//FIFO mode - continous , Threshold Setting 31 ( 31 values )
	config = 0xDF;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev))
	{
		LOG_ERR("Failed to send FIFo buffer configuration - I2C error");	
		ret =  ACCEL_ERROR;
	}
	else
	{
		printf("\rHERE \n\n");
		ret = i2c_write_bytes(i2c_dev, ACC_CONFIG_REGISTER_FIFO_CNTRL_ADDR, &config, ACC_FIFO_CONFIG_MSG_LEN, ACCEL_I2C_ADDR);
	}
	
	return ret;
}

/**
 * @brief Set Accelerometer Configuration
 * 
 * @return int error code
 */
int app_accel_config ()
{
	uint8_t config[ACC_CONFIG_MSG_LEN] ;
	// Reg 0x20 set mode to High-Performance / Low-Power mode 400/200 Hz 
	// High-Performance Mode (14-bit resolution)
	// 14 bit precision
	config[0] = 0x95;
	// default, but chaper to send than make two writes 
	config[1] = 0x04; 
	// Reg 0x22 0, use INT2 as TRIG
	config[2] = 0x01; 
	// Reg 0x23 use INT1 as FIFO Full threshold
	config[3] = 0x02; 
	
	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Unable to set IMU configuration - I2C device not ready");
		return ACCEL_ERROR;
	}

	
	k_work_init(&accelerometer_fifo_evt_work_item, accelerometer_events_work_handler);


	return  i2c_write_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL1_ADDR, config, ACC_CONFIG_MSG_LEN, ACCEL_I2C_ADDR);	 
}

/**
 * @brief Accelerometer trigger enable function
 * 
 */
void accel_trigger_enable(void)
{
  //The trigger requires a transition from Low to High. Trigger by default and after execution will go to Low State.
  //Setting High
  set_imu_trigger_pin();
}
	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));uint8_t accel_sensor_data[ACC_READ_DATA_BUFFER_SIZE];
/**
 * @brief Read accelerometer current value
 * 
 * @param accel_data Buffer to store current accelerometer data
 * @return int error code
 */
int app_accel_read(accel_data_t *accel_data)
{
	int accel_error = ACCEL_ERROR;


	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Unable to read IMU - I2C error");
	}
	else
	{
		
		accel_data->x_accel = 0x8000;
		accel_data->y_accel = 0x8000;
		accel_data->z_accel = 0x8000;

		int ret = i2c_read_bytes(i2c_dev, ACC_READ_DATA_REG_ADDR, accel_sensor_data, ACC_READ_DATA_BUFFER_SIZE, ACCEL_I2C_ADDR);

		if (ret) 
		{
			LOG_ERR("Unable to read IMU - IMU read error");
		} 
		else 
		{
			accel_data->x_accel = (int16_t)((accel_sensor_data[1] << 8) | accel_sensor_data[0]);
			accel_data->y_accel = (int16_t)((accel_sensor_data[3] << 8) | accel_sensor_data[2]);
			accel_data->z_accel = (int16_t)((accel_sensor_data[5] << 8) | accel_sensor_data[4]);

			LOG_INF("IMU: 0x%02X%02X 0x%02X%02X 0x%02X%02X", accel_sensor_data[1], accel_sensor_data[0], accel_sensor_data[3], accel_sensor_data[2], accel_sensor_data[5], accel_sensor_data[4]);
			accel_data->x_accel = (int16_t)((19600*(int32_t)accel_data->x_accel)>>15);  // 2g full scale, signed binary fraction, *9.8 m/s^2 per g * 1000
			accel_data->y_accel = (int16_t)((19600*(int32_t)accel_data->y_accel)>>15);
			accel_data->z_accel = (int16_t)((19600*(int32_t)accel_data->z_accel)>>15);
			LOG_INF( "IMU: %d, %d, %d m/s^2 * 1000", accel_data->x_accel, accel_data->y_accel, accel_data->z_accel);
			accel_error = ACCEL_SUCCESS;
		}
	}

	return accel_error;
}
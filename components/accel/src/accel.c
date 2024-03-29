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
#define ACC_CONFIG_REGISTER_CNTRL1_ADDR	0x20

#define ACC_CONFIG_REGISTER_CNTRL2_ADDR	0x22

#define ACC_DRDY_READY_TIMEOUT		100
#define ACC_READ_DATA_BUFFER_SIZE	6
#define ACC_READ_DATA_REG_ADDR		0x28

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
 * @brief Set Accelerometer Configuration
 * 
 * @return int error code
 */
int app_accel_config ()
{
	uint8_t initial[ACC_CONFIG_MSG_LEN];
	uint8_t config[ACC_CONFIG_MSG_LEN] ;
	// Reg 0x20 set mode to High-Performance / Low-Power mode 400/200 Hz 
	// Single data conversion on demand mode (12/14-bit resolution) [shutdown until told to trigger]
	// 12 bit precision
	config[0] = 0x78;
	// default, but chaper to send than make two writes 
	config[1] = 0x04; 
	#if (IMU_I2C_TRIGGER)
	config[2] = 0x02; // Reg 0x22 2, use I2C as TRIG
	#else
	config[2] = 0x00; // Reg 0x22 0, use INT2 as TRIG
	#endif
	#if (IMU_I2C_DRDY)
	config[3] = 0x00;
	#else
	config[3] = 0x01; // Reg 0x23 use INT1 as DRDY
	#endif 
	
	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Unable to set IMU configuration - I2C device not ready");
		return ACCEL_ERROR;
	}

	if (IMU_CONFIRM_REGS)
	{
		i2c_read_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL1_ADDR, initial, ACC_CONFIG_MSG_LEN, ACCEL_I2C_ADDR);
	}

	int ret =  i2c_write_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL1_ADDR, config, ACC_CONFIG_MSG_LEN, ACCEL_I2C_ADDR);

	if (IMU_CONFIRM_REGS)
	{
		i2c_read_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL1_ADDR, initial, ACC_CONFIG_MSG_LEN, ACCEL_I2C_ADDR);
	}
	return ret;
}

/**
 * @brief Accelerometer trigger enable function
 * 
 */
void accel_trigger_enable(void)
{

#if (IMU_I2C_TRIGGER)
	uint8_t initial = 0;
	uint8_t config = 0x01; // i2c trigger

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev))
	{
		LOG_ERR("Umable to enable IMU trigger - I2C device not ready");
		return;
	}

	if (IMU_CONFIRM_REGS)
	{
		i2c_read_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL2_ADDR, &initial, sizeof(config), ACCEL_I2C_ADDR);
	}

	i2c_write_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL2_ADDR, &config, sizeof(config), ACCEL_I2C_ADDR);

	if (IMU_CONFIRM_REGS)
	{
		i2c_read_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL2_ADDR, &initial, sizeof(config), ACCEL_I2C_ADDR);
	}
	return;
#else
  //The trigger requires a transition from Low to High. Trigger by default and after execution will go to Low State.
  //Setting High
  set_imu_trigger_pin();

#endif
}

/**
 * @brief Read the Data ready register/pin status
 * 
 * @return int Current status or error code in case of failure
 */
static int app_accel_drdy()
{
#if (IMU_I2C_DRDY)
	uint8_t resp = 0;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
	if (!device_is_ready(i2c_dev)) {
		LOG_ERR("Umable to read IMU DRDY status - I2C device not ready");
		return ACCEL_ERROR;
	}

	int ret = i2c_read_bytes(i2c_dev, ACC_CONFIG_REGISTER_CNTRL2_ADDR, &resp, sizeof(resp), ACCEL_I2C_ADDR);
	if (ret == ACCEL_SUCCESS)
	{
		ret = resp & 0x01;
	}

	return ret;
	#else
	return !get_imu_drdy_pin_status();
	#endif

}

/**
 * @brief Read accelerometer current value
 * 
 * @param accel_data Buffer to store current accelerometer data
 * @return int error code
 */
int app_accel_read(accel_data_t *accel_data)
{
	int accel_error = ACCEL_ERROR;
	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	uint16_t timeout = ACC_DRDY_READY_TIMEOUT;

	// Wait until DRDY pin is set, indicating the data is ready to read
	while (app_accel_drdy() && timeout--)
	{
		k_usleep (100);
	}
		
	// Check if timer expired and DRDY Pin was not set
	if (timeout == 0)
	{
		LOG_ERR("DRDY Timeout during IMU read");
	}
	else
	{
		if (!device_is_ready(i2c_dev)) 
		{
			LOG_ERR("Unable to read IMU - I2C error");
		}
		else
		{
			uint8_t accel_sensor_data[ACC_READ_DATA_BUFFER_SIZE];
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
				//LOG_INF( "IMU: %d, %d, %d m/s^2 * 1000", accel_data->x_accel, accel_data->y_accel, accel_data->z_accel);
				accel_error = ACCEL_SUCCESS;
			}
		}
	}
#if (!IMU_I2C_TRIGGER)
		//Setting Low 
		clear_imu_trigger_pin();
#endif
		return accel_error;
}

#if (USE_ZEPHYR_SENSOR)

static const enum sensor_channel channels[] = {
	SENSOR_CHAN_ACCEL_X,
	SENSOR_CHAN_ACCEL_Y,
	SENSOR_CHAN_ACCEL_Z,
};

/**
 * @brief Read Accelerometer data using Zephyr APIs
 * 
 * @param accel_data Buffer to store read data
 * @return int Error code
 */
int app_accel_service(accel_data_t *accel_data)
{
    const struct device *const accel_dev = DEVICE_DT_GET(DT_NODELABEL(accel));
    if (!device_is_ready(accel_dev)) {
		printk("Device %s is not ready\n", accel_dev->name);
		return ACCEL_ERROR;
	}

    int ret;
	struct sensor_value accel[3];
    
	ret = sensor_sample_fetch(accel_dev);
	if (ret < 0) {
		printk("%s: sensor_sample_fetch() failed: %d\n", accel_dev->name, ret);
		return ret;
	}

	for (size_t i = 0; i < ARRAY_SIZE(channels); i++) {
		ret = sensor_channel_get(accel_dev, channels[i], &accel[i]);
		if (ret < 0) {
			printk("%s: sensor_channel_get(%c) failed: %d\n", accel_dev->name, 'X' + i, ret);
			return ret;
		}
	}

    printk("[m/s^2]: (%12.6f, %12.6f, %12.6f)\n", sensor_value_to_double(&accel[0]), sensor_value_to_double(&accel[1]), sensor_value_to_double(&accel[2]));

    float val_x = sensor_value_to_double(&accel[0]);
    float val_y = sensor_value_to_double(&accel[1]);
    float val_z = sensor_value_to_double(&accel[2]);
    int16_t val_x_cast = (int16_t)(val_x*1000);
    int16_t val_y_cast = (int16_t)(val_y*1000);
    int16_t val_z_cast = (int16_t)(val_z*1000);

    printk("x: %i, y: %i, z: %i\n", val_x_cast, val_y_cast, val_z_cast);

    accel_data->x_accel = val_x_cast;
    accel_data->y_accel = val_y_cast;
    accel_data->z_accel = val_z_cast;

    return ACCEL_SUCCESS;
}
#endif
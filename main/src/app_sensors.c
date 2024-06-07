#include "app_sensors.h"
#include <zephyr/logging/log.h>
#include <hal/nrf_gpio.h>

#include "device_config.h"
#include "accel.h"
#include "app_gpio.h"

LOG_MODULE_DECLARE(wepower);


/**
 * @brief Routine used to measure the sensors data and store it in the buffer
 * 
 * @param we_power_data The pointer to the buffer on which the data is stored
 */
void measure_sensor_data(we_power_data_ble_adv_t *we_power_data)
{
    accel_data_t accel_data = {0};

    // If pressure sensor was not triggerd, trigger it
    set_CN1_7();

    // trigger accelerometer
    accel_trigger_enable();

    // initialize signed max negative as error codes in sensor data
    accel_data.x_accel = 0x8000;
	accel_data.y_accel = 0x8000;
	accel_data.z_accel = 0x8000;

    // read accel or error out (data set either way)
     if ( app_accel_read (&accel_data) != ACCEL_SUCCESS )
     {
        LOG_ERR ("Reading Accelerometer Data failed");
     }
 
    we_power_data->data_fields.accel_x.i16 = accel_data.x_accel;
    we_power_data->data_fields.accel_y.i16 = accel_data.y_accel;
    we_power_data->data_fields.accel_z.i16 = accel_data.z_accel;

    clear_CN1_7();
}
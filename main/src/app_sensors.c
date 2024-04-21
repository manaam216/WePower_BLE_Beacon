#include "app_sensors.h"
#include <zephyr/logging/log.h>
#include <hal/nrf_gpio.h>

#include "device_config.h"
#include "temp_pressure.h"
#include "accel.h"
#include "app_gpio.h"

LOG_MODULE_DECLARE(wepower);

/**
 * @brief This flag indicates if the pressure sensor was properly triggered or not
 * 
 */
bool is_temp_pressure_sensor_triggered = false;

/**
 * @brief Routine used to measure the sensors data and store it in the buffer
 * 
 * @param we_power_data The pointer to the buffer on which the data is stored
 */
void measure_sensor_data(we_power_data_ble_adv_t *we_power_data)
{
    accel_data_t accel_data = {0};
    temp_pressure_data_t temp_pressure_data = {0};

    // If pressure sensor was not triggerd, trigger it
    set_CN1_7();
    if (is_temp_pressure_sensor_triggered == false)
        is_temp_pressure_sensor_triggered = app_temp_pressure_trigger();

    // trigger accelerometer
    accel_trigger_enable();

    // initialize signed max negative as error codes in sensor data
    accel_data.x_accel = 0x8000;
	accel_data.y_accel = 0x8000;
	accel_data.z_accel = 0x8000;

    temp_pressure_data.pressure = 0x8000;
    temp_pressure_data.temp = 0x8000;

    // read accel or error out (data set either way)
     if ( app_accel_read (&accel_data) != ACCEL_SUCCESS )
     {
        LOG_ERR ("Reading Accelerometer Data failed");
     }
 
    we_power_data->data_fields.accel_x.i16 = accel_data.x_accel;
    we_power_data->data_fields.accel_y.i16 = accel_data.y_accel;
    we_power_data->data_fields.accel_z.i16 = accel_data.z_accel;

    // Get temp and pressure or error out
    is_temp_pressure_sensor_triggered = false;
    
    if ( app_temp_pressure_read (&temp_pressure_data) != TEMP_PRESSURE_SUCCESS )
    {
        LOG_ERR("Reading temp and pressure sensor data failed");
    }

    we_power_data->data_fields.pressure.i16 = temp_pressure_data.pressure;
    we_power_data->data_fields.temp.i16 = temp_pressure_data.temp;

    clear_CN1_7();
}
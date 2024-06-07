#ifndef __APP_SENSORS__
#define __APP_SENSORS__

#include <stdio.h>
#include "app_types.h"

/**
 * @brief Routine used to measure the sensors data and store it in the buffer
 * 
 * @param we_power_data The pointer to the buffer on which the data is stored
 */
void measure_sensor_data(we_power_data_ble_adv_t *we_power_data);

#endif // __APP_SENSORS__

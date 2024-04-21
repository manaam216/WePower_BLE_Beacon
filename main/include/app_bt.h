#ifndef __APP_BT__
#define __APP_BT__

#include <stdio.h>
#include <zephyr/kernel.h>

extern struct k_work start_advertising_work_item;

/**
 * @brief BT ready function. Should be called after bluetooth is enabled
 *        successfully. 
 * 
 * @return int error code
 */
int bt_ready(void);

/**
 * @brief Function used to start the advertising on Bluetooth
 * 
 * @param work Work Item from work queue
 */
void start_advertising_handler(struct k_work *work);

/**
 * @brief Initialize Bluetooth for WePower Board
 * 
 * @return uint8_t error code, 0 if successful
 */
uint8_t initialize_bluetooth();

#endif // __APP_BT__

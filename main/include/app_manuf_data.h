#ifndef __APP_MANUF_DATA__
#define __APP_MANUF_DATA__

#include <zephyr/kernel.h>
#include "device_config.h"

extern uint8_t manufacture_data[PAYLOAD_FRAME_LENGTH];
extern uint8_t TX_Repeat_Counter;

/**
 * @brief Update the manufacture data
 * 
 * @note only call this ONCE per EventCounter (FRAM[0:3])
 * 
 */
void update_manufacture_data(void);

#endif // __APP_MANUF_DATA__

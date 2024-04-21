#include "app_manuf_data.h"

#include <stdio.h>
#include <string.h>

#include <zephyr/logging/log.h>
#include <hal/nrf_gpio.h>

#include "app_types.h"
#include "encrypt.h"
#include "config_commands.h"

#include "app_encrypt.h"
#include "app_sensors.h"

LOG_MODULE_DECLARE(wepower);

typedef enum
{
	DATA_TYPE_SENSOR_DATA_0 = 0,
	DATA_TYPE_SENSOR_DATA_1,
	DATA_TYPE_SENSOR_DATA_2,
	DATA_TYPE_POLARITY_AND_NAME_9_BYTES,
	DATA_TYPE_NAME_10_BYTES,
}fram_data_type_t;

/*                                                             |--------------------------------- --------- ENCRYPTED  DATA -------------------------------- | |-- ID --| status,cnt */
uint8_t manufacture_data[PAYLOAD_FRAME_LENGTH] = { 0x50, 0x57, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

extern uint8_t u8Polarity;

/**
 * @brief 16 bytes of encrypted data, type dependent.
 * 
 * Encrypted payload varies with type, which is always first byte of 16.
 * Next three bytes are the least significant 24 of 32 byte event counter from FRAM.
 * The next 10 bytes are type dependent:
 * Types 0-2:
                6 bytes of IMU data, as 16 bit signed fraction of +/-16g.
                4 bytes of pressure (signed kPa x16) and temperature (signed C x 10).
 * Type 3:
                Polarity byte
                 - taken at boot from an external comparator and used by some sensor types to determine switch direction.
				9 character name/msg.
 * Type 4:
				10 character name/msg....
 *
 * 2-least significant bytes of serial number (is in both encrypted data and outer framing) 
 */
we_power_data_ble_adv_t we_power_data;


// 
/**
 * @brief Used to allow the recipient to guess how many packets were sent in an event.
 * 
 * @note "guess" because some packets will be lost and the last one received may not be the last one sent.
 * 
 */
uint8_t TX_Repeat_Counter = TX_REPEAT_COUNTER_DEFAULT_VALUE;

/**
 * @brief Update the manufacture data
 * 
 * @note only call this ONCE per EventCounter (FRAM[0:3])
 * 
 */
void update_manufacture_data(void)
{
    LOG_INF(">>> Updating the Manufacturer Data");
	static uint8_t cipher_text[DATA_SIZE_BYTES];
   //Get sensor data
	switch (fram_data.type)
	{
		case DATA_TYPE_SENSOR_DATA_0:
		case DATA_TYPE_SENSOR_DATA_1:
		case DATA_TYPE_SENSOR_DATA_2:
			measure_sensor_data(&we_power_data);
			break;

		case DATA_TYPE_POLARITY_AND_NAME_9_BYTES: 
			we_power_data.data_bytes[4] = u8Polarity;
			memcpy(&we_power_data.data_bytes[5], fram_data.cName, 9);
			break;

		case DATA_TYPE_NAME_10_BYTES: 
			memcpy(&we_power_data.data_bytes[4], fram_data.cName, 10);
			break;

		default: 
			// do nothing
			break;
	}

    // increase the FRAM Event counter and set first four bytes
	fram_data.event_counter++;
    app_fram_write_counter(&fram_data);

	we_power_data.data_fields.type = fram_data.type;
	we_power_data.data_fields.event_counter24[0] = (uint8_t)((fram_data.event_counter & 0x000000FF));
	we_power_data.data_fields.event_counter24[1] = (uint8_t)((fram_data.event_counter & 0x0000FF00)>>8);
	we_power_data.data_fields.event_counter24[2] = (uint8_t)((fram_data.event_counter & 0x00FF0000)>>16);
    we_power_data.data_fields.id.u16 = fram_data.serial_number & 0xFFFF;

    // initialize the TX counter

	TX_Repeat_Counter = TX_REPEAT_COUNTER_DEFAULT_VALUE;
	manufacture_data[PAYLOAD_TX_REPEAT_COUNTER_INDEX] = TX_Repeat_Counter;
   
    // Handle encryption
    encrypt_data(we_power_data.data_bytes, cipher_text, PAYLOAD_DATA_SIZE_BYTES);

    // Build the BLE adv data
    memcpy(&manufacture_data[PAYLOAD_DATA_START_INDEX], cipher_text,                PAYLOAD_DATA_SIZE_BYTES);
    memcpy(&manufacture_data[PAYLOAD_DEVICE_ID_INDEX],  &(fram_data.serial_number), PAYLOAD_SERIAL_NUMBER_SIZE);
}
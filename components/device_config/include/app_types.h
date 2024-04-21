#ifndef __APP_TYPES__
#define __APP_TYPES__

#include <stdint.h>

#define DATA_SIZE_BYTES 16
#define EVENT_COUNTER_NUM_BYTES 3

/**
 * @brief Union for uint32_t and uint8_t * 4.
 * This will be used for FRAM data
 * 
 */
typedef union {
    uint32_t u32;
    uint8_t u8[sizeof(uint32_t)];
} u32_u8_t;

/**
 * @brief Union for int32_t and uint8_t * 4.
 * 
 */
typedef union {
    int32_t i32;
    uint8_t u8[sizeof(int32_t)];
} i32_u8_t;

/**
 * @brief Union for uint16_t and uint8_t * 2.
 * This will be used store the id
 * 
 */
typedef union {
    uint16_t u16;
    uint8_t u8[sizeof(uint16_t)];
} u16_u8_t;


/**
 * @brief Union for int16_t and uint8_t * 2.
 * This will be used to store the sensor values and store them in FRAM.
 * 
 */
typedef union {
    int16_t i16;
    uint8_t u8[sizeof(int16_t)];
} i16_u8_t;

/**
 * @brief We power data struct. This shows the information that will be
 * sent over BLE as advertising data
 * 
 */
typedef struct 
{
	uint8_t type;                                       // Type of the sensor
    uint8_t event_counter24[EVENT_COUNTER_NUM_BYTES];   // Number of events counter
    i16_u8_t accel_x;                                   // Accelerometer X axis value    
    i16_u8_t accel_y;                                   // Accelerometer y axis value
    i16_u8_t accel_z;                                   // Aceelerometer z axis value
    i16_u8_t temp;                                      // Temperature value from sensor
    i16_u8_t pressure;                                  // Pressue value from sensor
    u16_u8_t id;                                        // ID
} we_power_data_t;

/**
 * @brief Union for sending the we power data over BLE Advertising. 
 * The reason for using the uninon is to make the byte array accessible instead of type casting
 * 
 */
typedef union {
    we_power_data_t data_fields;
    uint8_t data_bytes[DATA_SIZE_BYTES];
} we_power_data_ble_adv_t;

#endif // __APP_TYPES__

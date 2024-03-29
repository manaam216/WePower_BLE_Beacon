#ifndef __CONFIG_COMMANDS__
#define __CONFIG_COMMANDS__

#include "fram.h"
#include <zephyr/device.h>

#define UART_MSG_SIZE               512                             // UART Message Size
#define FIELD_NAME_MAX_LENGTH       32                              // Maximum length of field name        
#define UART_DEVICE_NODE            DT_CHOSEN(zephyr_shell_uart)    // UART Device Node from Device tree

extern fram_data_t fram_data;

typedef enum 
{  
    DEVICE_TYPE_LEGACY = 0,         // Legacy but behaves same as 1
    DEVICE_TYPE_BUTTON,             // Button.  Intends to do one burst of packets per event with no polarity
    DEVICE_TYPE_VIBRATION_MONITOR,  // Vibration monitor, periodic bursts of packets, no polarity.  Future --> pair and stream
    DEVICE_TYPE_TWO_WAY_SWITCH,     // Two-way switch with polarity and name in payload
    DEVICE_TYPE_RELEASE_SENSOR,     // Prewound release sensor with just a name
    DEVICE_TYPE_MAX_VALUE           // Max number of Device Types
}device_type_t;

/**
 * @brief Enumeration of the types of command which can be received
 * 
 */
typedef enum
{
    COMMAND_TYPE_GET = 0,
    COMMAND_TYPE_SET,
    COMMAND_TYPE_RESET,
    COMMAND_TYPE_DUMP,
    COMMAND_TYPE_PRESET
}command_type_t;

/**
 * @brief Enumeration for types of the data stored in FRAM
 * 
 */
typedef enum 
{
    DATA_NUMBER = 0,
    DATA_STRING,
    DATA_BYTE_ARRAY,
}fram_data_format_t;

/**
 * @brief Enumeration for preset type
 * 
 */
typedef enum
{
    PRESET_TYPE_BUTTON_0  = 0,
    PRESET_TYPE_BUTTON_1  = 1,
    PRESET_TYPE_VIB_SENS  = 2,
    PRESET_TYPE_ON_OFF_SW = 3,
    PRESET_TYPE_GENERATOR = 4
}preset_type_t;


/**
 * @brief Struct for the format of the UART Commands
 * 
 */
typedef struct
{
    uint8_t type;                       // Type of the command received ( command_type_t )
    int8_t field_index;                 // Field Index on which we will do business 
    uint8_t data_len;                   // Received Data length 
    uint8_t data[(UART_MSG_SIZE - 3)];  // Recevied Data ( -3 indicates omitting the type, index and data len values inside the buffer)
}command_data_t;

/**
 * @brief Structure used to store information regarding different FRAM Fields
 * 
 */
typedef struct
{
    char name[FIELD_NAME_MAX_LENGTH];   // Name of the Field
    uint8_t type;                       // Type of the Device (device_type_t)    
    uint8_t field_length;               // Length of the Field 
    uint32_t min_value;                 // Minimum value which can be stored in the field 
    uint32_t max_value;                 // maxmimum value which can be stored in the field
    uint32_t default_value;             // Default Value stored in the field - usually used at reset command
}fram_info_t;

/**
 * @brief Function to initialize the UART functionality
 * 
 * @return int Error code
 */
int init_uart(void);

/**
 * @brief Print the buffer to UART 0
 * 
 * @param buffer Buffer containing the data to print
 */
void print_uart(char *buffer);

/**
 * @brief Process Command function
 * 
 * @param work Work Item to process
 */
void process_command_fn(struct k_work *work);

/**
 * @brief Function to dump content to FRAM
 * 
 * @param print Flag to indicate if printing should be done
 * @return int32_t error code
 */
int32_t dump_fram(uint8_t print);

#endif // __CONFIG_COMMANDS__

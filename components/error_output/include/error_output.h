#ifndef __ERROR_OUTPUT__
#define __ERROR_OUTPUT__

#include <stdint.h>

/**
 * @brief Enumeration for the types of error which can occur
 * 
 */
typedef enum
{
    ERROR_TYPE_UART = 0,
    ERROR_TYPE_FRAM = 1,
    ERROR_TYPE_BT_ENABLE_FAIL = 2,
    ERROR_TYPE_BT_READY_FAIL = 3
}error_types_t;

/**
 * @brief Set the defualt state of error pin i.e LOW
 * 
 */
void set_default_state_of_error_pin();

/**
 * @brief Indicate the error type
 * 
 * @param error_type_to_indicate Error type to indicate
 */
void indicate_error(uint16_t error_type_to_indicate);

#endif // __ERROR_OUTPUT__

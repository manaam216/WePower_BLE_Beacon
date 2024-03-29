#ifndef __COMPARATOR__
#define __COMPARATOR__

#include <stdio.h>
#include <stdint.h>

#define AIN_CHANNEL_FOR_COMP_1  1
#define AIN_CHANNEL_FOR_COMP_2  2

/**
 * @brief Initialize the comparator 1 of the module and read the voltage
 * 
 * @return uint8_t Initial value from the comparator
 */
uint8_t init_comparator_1_vext_and_read_value();

/**
 * @brief Initialize the comparator 2 of the module
 * 
 */
void init_comparator_2_vbulk();

#endif // __COMPARATOR__

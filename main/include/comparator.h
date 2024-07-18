#ifndef __COMPARATOR__
#define __COMPARATOR__

#include <stdio.h>
#include <stdint.h>

#define AIN_CHANNEL_FOR_COMP_1  1
#define AIN_CHANNEL_FOR_COMP_2  2
#define AIN_POS_CHANNEL_FOR_DIFF_COMPARATOR 4
#define AIN_NEG_CHANNEL_FOR_DIFF_COMPARATOR 7

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

/**
 * @brief Get the current value of the comparator 2
 * 
 * @return uint8_t read value at comparator 2
 */
uint8_t get_comaprator_2_current_value();

uint8_t init_differential_comparator();

#endif // __COMPARATOR__

#ifndef __COMPARATOR__
#define __COMPARATOR__

#include <stdio.h>
#include <stdint.h>

#define AIN_CHANNEL_FOR_COMP_1  1
#define AIN_CHANNEL_FOR_COMP_2  2
#define AIN_POS_CHANNEL_FOR_DIFF_COMPARATOR 4
#define AIN_NEG_CHANNEL_FOR_DIFF_COMPARATOR 7

/**
 * @brief Get application mode
 * 
 * @return uint8_t True if UART Mode 0 otherwise
 */
uint8_t get_application_mode();

/**
 * @brief Initialize the comparator 2 of the module
 * 
 */
uint8_t init_comparator_2_vbulk();

/**
 * @brief Get the current value of the comparator 2
 * 
 * @return uint8_t read value at comparator 2
 */
uint8_t get_comaprator_2_current_value();

/**
 * @brief Initialize Differential Comparator for Measuring Polarity
 * 
 * @return uint8_t Output of the differential Comparotor ( 1 or 0 )
 */
uint8_t init_differential_comparator();

#endif // __COMPARATOR__

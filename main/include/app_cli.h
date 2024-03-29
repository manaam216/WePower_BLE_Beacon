#ifndef __APP_CLI__
#define __APP_CLI__

#include <zephyr/kernel.h>


/**
 * @brief Initialize the command line interface for receiving commands via UART
 * 
 * @return uint8_t error code
 */
uint8_t init_command_line_interface();

#endif // __APP_CLI__

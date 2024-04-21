#include "error_output.h"

#include <zephyr/kernel.h>
#include <hal/nrf_gpio.h>

#include "nrfx_log.h"

#include "device_config.h"
#include "app_gpio.h"

K_MUTEX_DEFINE(error_mutex);        // This will ensure thread safety

/**
 * @brief Enumeration for the states of error pin
 * 
 */
typedef enum
{
    ERROR_PIN_LOW   = 0,
    ERROR_PIN_HIGH  = 1,
}error_pin_states_t;

/**
 * @brief Toggle the error pin and set the default state after toggle
 * 
 * @param delay_msec Delay during consective toggles
 */
static void toggle_error_pin(uint16_t delay_msec)
{
    uint16_t no_of_toggles = MAX_NUMBER_OF_TOGGLES_FOR_ERROR_INDICATION;
    while ( no_of_toggles-- )
    {
        toggle_CN_1_4();
        k_msleep(delay_msec);
    }
    set_default_state_of_error_pin();
}

/**
 * @brief Set the defualt state of error pin i.e LOW
 * 
 */
void set_default_state_of_error_pin()
{
    clear_CN1_4();;
}

/**
 * @brief Indicate the error type
 * 
 * @param error_type_to_indicate Error type to indicate
 */
void indicate_error(uint16_t error_type_to_indicate)
{
    if ( k_mutex_lock(&error_mutex, ERROR_MUTEX_WAIT_TIME_MSEC))
    {
        switch (error_type_to_indicate)
        {
            case ERROR_TYPE_UART:
            {
                toggle_error_pin(UART_ERROR_DELAY_MSEC);
                break;
            }
            case ERROR_TYPE_FRAM:
            {
                toggle_error_pin(FRAM_ERROR_DELAY_MSEC);
                break;
            }
            case ERROR_TYPE_BT_ENABLE_FAIL:
            {
                toggle_error_pin(BT_ENABLE_ERROR_DELAY_MSEC);
                break;
            }
            case ERROR_TYPE_BT_READY_FAIL:
            {
                toggle_error_pin(BT_READY_ERROR_DELAY_MSEC);
                break;
            }
        
        default:
            break;
        }
        k_mutex_unlock(&error_mutex);
    }
    else
    {
        NRFX_LOG_ERROR(" Unable to accquire error mutex %d", __LINE__);
    }
}

#include "comparator.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrfx_comp.h>
#include <hal/nrf_gpio.h>

#include "app_burn_energy.h"
#include "config_commands.h"

#define COMPARATOR_1_VDOWN_VOLTAGE      0.8
#define COMPARATOR_1_VUP_VOLTAGE        1.0
#define COMPARATOR_1_REFERENCE_VOLTAGE  1.2

#define COMPARATOR_2_REFERENCE_VOLTAGE  1.2

#define COMPARTATOR_REFERENCE NRF_COMP_REF_Int1V2

#define COMPARATOR_INTERRUPT_PRIORITY   5

LOG_MODULE_DECLARE(wepower);

/**
 * @brief Handler for comparator related events
 * 
 * @param event 
 */
static void comparator_handler (nrf_comp_event_t event)
{

    switch (event)
    {
    case NRF_COMP_EVENT_DOWN:
        /* code */
        break;
    case NRF_COMP_EVENT_UP:
        /* code */
        break;
    
    default:
        break;
    }
    nrfx_comp_stop();
    nrfx_comp_uninit();
	// called when VBULK falls below 2V
    // switching cap should have triggered at >2V so this is a real death
    burn_the_energy();
}

/**
 * @brief Get application mode
 * 
 * @return uint8_t True if UART Mode 0 otherwise
 */
uint8_t get_application_mode()
{
    nrf_gpio_cfg_input (3, NRF_GPIO_PIN_NOPULL);

    k_usleep (10);
    
    return nrf_gpio_pin_read (3);
}


/**
 * @brief Initialize the comparator 2 of the module
 * 
 */
uint8_t init_comparator_2_vbulk()
{
    nrfx_comp_config_t  comp_config = NRFX_COMP_DEFAULT_CONFIG(AIN_CHANNEL_FOR_COMP_2);
    nrf_comp_th_t thresh;
    float thresh_value =(((float)fram_data.vbulk_thresh)/100.0);
    comp_config.reference = COMP_REFSEL_REFSEL_Int1V2;
    comp_config.speed_mode = NRF_COMP_SP_MODE_High; // Enables the low power mode 

    thresh.th_down = NRFX_VOLTAGE_THRESHOLD_TO_INT(thresh_value, COMPARATOR_2_REFERENCE_VOLTAGE);
    thresh.th_up   = NRFX_VOLTAGE_THRESHOLD_TO_INT(thresh_value, COMPARATOR_2_REFERENCE_VOLTAGE);
    comp_config.threshold = thresh;
    
    if ( nrfx_comp_init(&comp_config, NULL) != NRFX_SUCCESS )
    {
        LOG_ERR("Unable to init the comparator for VBulk pin");
    }

    nrfx_comp_start(0,  0);

    return nrfx_comp_sample();
}

/**
 * @brief Get the current value of the comparator 2
 * 
 * @return uint8_t read value at comparator 2
 */
uint8_t get_comaprator_2_current_value()
{
    init_comparator_2_vbulk();
    return nrfx_comp_sample();
}

uint8_t init_differential_comparator()
{
    // nrfx_comp_config_t  comp_config = NRFX_COMP_DEFAULT_CONFIG(AIN_POS_CHANNEL_FOR_DIFF_COMPARATOR);
    // // comp_config.reference = NRF_COMP_REF_Int1V2; 
    // comp_config.main_mode = COMP_MODE_MAIN_Diff;
    // comp_config.ext_ref = AIN_NEG_CHANNEL_FOR_DIFF_COMPARATOR;
    // comp_config.speed_mode = COMP_MODE_SP_Low;
    // nrfx_comp_init(&comp_config, NULL);
	// nrfx_comp_start(0,  0);

    return nrfx_comp_sample();
}

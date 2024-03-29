#include "comparator.h"

#include <zephyr\kernel.h>
#include <zephyr/logging/log.h>
#include <nrfx_comp.h>

#include "app_burn_energy.h"

#define COMPARATOR_1_VDOWN_VOLTAGE      0.8
#define COMPARATOR_1_VUP_VOLTAGE        1.0
#define COMPARATOR_1_REFERENCE_VOLTAGE  1.2

#define COMPARATOR_2_VDOWN_VOLTAGE      0.8
#define COMPARATOR_2_VUP_VOLTAGE        1.0
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
	// called when VBULK falls below 1.86V
    // switching cap should have triggered at >2V so this is a real death
    burn_the_energy();
}

/**
 * @brief Initialize the comparator 1 of the module and read the voltage
 * 
 * @return uint8_t Initial value from the comparator
 */
uint8_t init_comparator_1_vext_and_read_value()
{
    uint8_t buffer_to_store_sample[5] = {0,0,0,0,0};
    bool is_reading_in_progress = false;
    nrfx_comp_config_t  comp_config = NRFX_COMP_DEFAULT_CONFIG(AIN_CHANNEL_FOR_COMP_1);
    nrf_comp_th_t thresh;
    comp_config.reference = COMPARTATOR_REFERENCE;
    comp_config.speed_mode = NRF_COMP_SP_MODE_Low; // Enables the low power mode 
    comp_config.interrupt_priority = COMPARATOR_INTERRUPT_PRIORITY;

    thresh.th_down = NRFX_VOLTAGE_THRESHOLD_TO_INT(COMPARATOR_1_VDOWN_VOLTAGE, COMPARATOR_1_REFERENCE_VOLTAGE);
    thresh.th_up   = NRFX_VOLTAGE_THRESHOLD_TO_INT(COMPARATOR_1_VUP_VOLTAGE, COMPARATOR_1_REFERENCE_VOLTAGE);
    comp_config.threshold = thresh;
    
    if ( nrfx_comp_init(&comp_config, comparator_handler) != NRFX_SUCCESS )
    {
        LOG_ERR("Unable to init the comparator 1 for Vext pin");
        buffer_to_store_sample[0] = 0xFF;
        goto exit;
    }

    nrfx_comp_start(0,  0);

    k_usleep (100);

    is_reading_in_progress = true;
    while (is_reading_in_progress)
    {
        buffer_to_store_sample[0] = nrfx_comp_sample();
        for (uint8_t sample_buffer_idx = 1; sample_buffer_idx < 5; sample_buffer_idx++)
        {
            buffer_to_store_sample[sample_buffer_idx] = nrfx_comp_sample();
            if (buffer_to_store_sample[0] == buffer_to_store_sample[sample_buffer_idx])
            {
                // do nothing
            }
            else
            {
                /**
                 * @brief If is_reading_in_progress is true, then set to false, and vice versa
                 * 
                 */
                is_reading_in_progress = !is_reading_in_progress;
                break;
            }
        }
        /**
         * @brief If is_reading_in_progress is true, then set to false, and vice versa.
         *        This will ensure the while loop runs again if there is any error in the reading.
         *        IF all values are OK, then the while loop while end.
         * 
         */
        is_reading_in_progress = !is_reading_in_progress;
    }
    
    exit:
    nrfx_comp_stop();
    nrfx_comp_uninit();
    return buffer_to_store_sample[0];
}


/**
 * @brief Initialize the comparator 2 of the module
 * 
 */
void init_comparator_2_vbulk()
{
    nrfx_comp_config_t  comp_config = NRFX_COMP_DEFAULT_CONFIG(AIN_CHANNEL_FOR_COMP_2);
    nrf_comp_th_t thresh;
    comp_config.reference = COMPARTATOR_REFERENCE;
    comp_config.speed_mode = NRF_COMP_SP_MODE_Low; // Enables the low power mode 
    comp_config.interrupt_priority = COMPARATOR_INTERRUPT_PRIORITY;

    thresh.th_down = NRFX_VOLTAGE_THRESHOLD_TO_INT(COMPARATOR_2_VDOWN_VOLTAGE, COMPARATOR_2_REFERENCE_VOLTAGE);
    thresh.th_up   = NRFX_VOLTAGE_THRESHOLD_TO_INT(COMPARATOR_2_VUP_VOLTAGE, COMPARATOR_2_REFERENCE_VOLTAGE);
    comp_config.threshold = thresh;
    
    if ( nrfx_comp_init(&comp_config, comparator_handler) != NRFX_SUCCESS )
    {
        LOG_ERR("Unable to init the comparator for VBulk pin");
    }

    nrfx_comp_start(NRFX_COMP_EVT_EN_DOWN_MASK,  0);
}

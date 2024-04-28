#include <zephyr/kernel.h>
#include <nrfx_comp.h>
#include <hal/nrf_gpio.h>

#define TEST_AIN_CHANNEL_FOR_COMP 1 // p0.03
#define TEST_PIN_OUTPUT 23

#define DELAY_MS        500

uint32_t temp_comp_buffer = 0;

int main(void)
{
        nrfx_comp_config_t  comp_config = NRFX_COMP_DEFAULT_CONFIG(TEST_AIN_CHANNEL_FOR_COMP);
        // comp_config.reference = NRF_COMP_REF_Int1V2; 
        comp_config.main_mode = COMP_MODE_MAIN_Diff;
        comp_config.ext_ref = NRF_COMP_EXT_REF_3;
        comp_config.speed_mode = COMP_MODE_SP_Low;
        nrfx_comp_init(&comp_config, NULL);

        nrf_gpio_cfg_output(TEST_PIN_OUTPUT);

	nrfx_comp_start(0,  0);

        (void)nrfx_comp_sample();

        while (1)
        {
                
                

                temp_comp_buffer = nrfx_comp_sample();

                if (temp_comp_buffer)
                {
                        printk("\r Its gone High Again \n");
                }
                
                nrf_gpio_pin_write(TEST_PIN_OUTPUT, temp_comp_buffer);

                k_usleep(10);
        }
        
        return 0;
}

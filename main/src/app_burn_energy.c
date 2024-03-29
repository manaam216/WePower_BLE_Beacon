#include "app_burn_energy.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <hal/nrf_gpio.h>
#include <zephyr/logging/log.h>
#include <nrfx_comp.h>

#include "device_config.h"
#include "config_commands.h"
#include "i2c_sensors.h"
#include "app_gpio.h"

#define USE_I2C_BURN_ENERGY 			1 // True if using I2C for burn energy, 0 if using GPIO 
#define VOLT_REGULATOR_I2C_ADDR 		0x1C
#define VOLT_REGULATOR_CONFIG_NUM_BYTES	1
#define VOLT_REGULATOR_CONFIG_REG_ADDR	0x13
#define VOLT_REGULATOR_CONFIG_DATA		0x20

LOG_MODULE_DECLARE(wepower);

/**
 * @brief Routine used to indicate I2C error on the GPIO pin
 * 
 */
static void indicate_i2c_fail_on_gpio()
{
    uint8_t toggles = MAX_NUMBER_OF_TOGGLES_FOR_ERROR_INDICATION;
    while (toggles--)
	{ 
        toggle_CN_1_5();
	}
    LOG_ERR("Burn Energy - I2C device is not ready");
}


/**
 * @brief Routine used to draw power to kill the energy
 * 
 */
void burn_the_energy(void)
{

#if USE_I2C_BURN_ENERGY
	uint8_t config = VOLT_REGULATOR_CONFIG_DATA; // Reg 0x13, 0x20 enables switched aux out

    nrfx_comp_stop();

	// write to dev 0x1c, ISL9123 buck reg, register 0x13, one byte.

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));
	if (!device_is_ready(i2c_dev)) 
	{
        indicate_i2c_fail_on_gpio();
		return;
	}

	i2c_write_bytes(i2c_dev, VOLT_REGULATOR_CONFIG_REG_ADDR, &config, VOLT_REGULATOR_CONFIG_NUM_BYTES, VOLT_REGULATOR_I2C_ADDR);
#endif

clear_CN1_5();
  
}
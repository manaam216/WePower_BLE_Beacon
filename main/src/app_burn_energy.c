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
 * @brief Routine used to draw power to kill the energy
 * 
 */
void burn_the_energy(void)
{
	clear_CN1_5();
}
#ifndef __APP_GPIO__
#define __APP_GPIO__

#include <stdint.h>
#include "zephyr/drivers/gpio.h"

typedef void (*gpio_intr_cb_ptr)(const struct device*, struct gpio_callback*, uint32_t );

/**
 * @brief Initialize the GPIOs of the WePower Board
 * 
 * @return  
 */
int init_we_power_board_gpios(void);

/**
 * @brief Open the polarity reading window and read the GPIO. 
 *        Sleep for the specified time then read GPIO polarity of POL_GPIO_PIN
 * 
 * @note Sleep time is targeting a stable point in the harvester AC waveform.
 * @note 2ms seems good for slide/water/limit switch engine but depends on code to this point.
 * 
 * @param sleep_time Time to sleep, before reading the GPIO
 * @return uint8_t If successful, will return the read polarity value
 */
uint8_t read_polarity(uint16_t sleep_time);

/**
 * @brief Toggle the Connector 1 pin #6
 * 
 */
void toggle_CN_1_6();

/**
 * @brief Clear the Connector 1 pin #6
 * 
 */
void clear_CN1_6();

/**
 * @brief Set the Connector 1 pin #6
 * 
 */
void set_CN1_6();

/**
 * @brief Toggle the connector 1 pin #4
 * 
 */
void toggle_CN_1_4();

/**
 * @brief Clear the trigger pin of IMU
 * 
 */
void clear_imu_trigger_pin();

/**
 * @brief Set the imu trigger pin 
 * 
 */
void set_imu_trigger_pin();

/**
 * @brief Get the imu drdy pin status
 * 
 * @return uint8_t Current status of the DRDY pin of IMU
 */
uint8_t get_imu_int2_pin_status();

/**
 * @brief Get the temperature and presseure sensor drdy pin status
 * 
 * @return uint8_t Current status of the DRDY pin of TPS sensor
 */
uint8_t get_tps_drdy_pin_status();

/**
 * @brief Set the Connector 1 pin #7
 * 
 */
void set_CN1_7();

/**
 * @brief Clear the Connector 1 pin #7
 * 
 */
void clear_CN1_7();

/**
 * @brief Set the Connector 1 pin #5
 * 
 */
void set_CN1_5();

/**
 * @brief Clear the Connector 1 pin #5
 * 
 */
void clear_CN1_5();

/**
 * @brief Toggle the Connector 1 pin #7
 * 
 */
void toggle_CN_1_7();

/**
 * @brief Toggle the Connector 1 pin #5
 * 
 */
void toggle_CN_1_5();

/**
 * @brief Clear the Connector 1 pin #4
 * 
 */
void clear_CN1_4();

/**
 * @brief Set the Connector 1 pin #4
 * 
 */
void set_CN1_4();

/**
 * @brief Configure the interurpt for INT2 pin of accelerometer
 * 
 * @param int2_cb Callback function for GPIO
 */
void configure_interrupt_for_accel_int2_pin(gpio_intr_cb_ptr int2_cb);

/**
 * @brief Configure GPIO interrupt for specified pin
 * 
 * @param gpio GPIO to configure the interrupt for
 * @param gpio_interrupt_callback Callback function for GPIO interrupt
 */
void configure_interrupt_for_gpio_pin(const struct gpio_dt_spec gpio, gpio_intr_cb_ptr gpio_interrupt_callback);

/**
 * @brief Enable accelerometer interrupts
 * 
 */
void enable_accel_int2_interrupts();

/**
 * @brief Enable interrupt for a specific GPIO pin
 * 
 * @param gpio GPIO pin to enable interrupt for 
 */
void enable_pin_interrupts(const struct gpio_dt_spec gpio);

/**
 * @brief Disable accelerometer INT2 interrupts
 * 
 */
void disable_accel_int2_interrupts();

/**
 * @brief Disbale interrupt for a specific GPIO pin
 * 
 * @param gpio GPIO pin to disable interrupt for 
 */
void disable_pin_interrupts(const struct gpio_dt_spec gpio);

#endif // __APP_GPIO__

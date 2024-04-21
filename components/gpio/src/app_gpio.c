#include "app_gpio.h"
#include <zephyr/kernel.h>
#include <hal/nrf_gpio.h>
#include "zephyr/drivers/gpio.h"

#include "device_config.h"

/**
 * @brief Device tree specification for IMU trigger pin GPIO
 * 
 */
static const struct gpio_dt_spec imu_trig    = GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(lis_trig),gpios,0);

/**
 * @brief Device tree specification for IMU data ready pin GPIO
 * 
 */
static const struct gpio_dt_spec imu_drdy    = GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(imu_drdy),gpios,0);

/**
 * @brief Device tree specification for polarity pin GPIO
 * 
 */
static const struct gpio_dt_spec polarity_pin    = GPIO_DT_SPEC_GET(DT_NODELABEL(polarity),gpios);

/**
 * @brief Device tree specification for connector pin # 4
 * 
 */
static const struct gpio_dt_spec connector_pin_4 = GPIO_DT_SPEC_GET(DT_NODELABEL(connector4),gpios);

/**
 * @brief Device tree specification for connector pin # 5
 * 
 */
static const struct gpio_dt_spec connector_pin_5 = GPIO_DT_SPEC_GET(DT_NODELABEL(connector5),gpios);

/**
 * @brief Device tree specification for connector pin # 6
 * 
 */
static const struct gpio_dt_spec connector_pin_6 = GPIO_DT_SPEC_GET(DT_NODELABEL(connector6),gpios);

/**
 * @brief Device tree specification for connector pin # 7
 * 
 */
static const struct gpio_dt_spec connector_pin_7 = GPIO_DT_SPEC_GET(DT_NODELABEL(connector6),gpios);

/**
 * @brief Device tree specification for temperature and pressure sensor DRDY pin
 * 
 */
static const struct gpio_dt_spec tps_drdy = GPIO_DT_SPEC_GET(DT_NODELABEL(lps_drdy),gpios);

/**
 * @brief Initialize the GPIOs of the WePower Board
 * 
 * @return  0
 */
int init_we_power_board_gpios(void)
{
    gpio_pin_configure_dt(&imu_drdy, GPIO_INPUT);
    gpio_pin_configure_dt(&tps_drdy, GPIO_INPUT);

    gpio_pin_configure_dt(&imu_trig, GPIO_OUTPUT);
    clear_imu_trigger_pin();

    gpio_pin_configure_dt(&polarity_pin, GPIO_INPUT | GPIO_PULL_UP);
    set_CN1_5();

    gpio_pin_configure_dt(&connector_pin_4, GPIO_OUTPUT);
    gpio_pin_configure_dt(&connector_pin_5, GPIO_OUTPUT);
    gpio_pin_configure_dt(&connector_pin_6, GPIO_OUTPUT);
    gpio_pin_configure_dt(&connector_pin_7, GPIO_OUTPUT);
    return 0;
}

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
uint8_t read_polarity(uint16_t sleep_time)
{
    uint8_t read_polarity = 0xFF;
    // Start thye polarity reading window
    set_CN1_6();
    // Sleep for the specific time
    k_sleep(K_MSEC(sleep_time));  
    // read the pin state, after configuring it to pull_up
    gpio_pin_configure_dt(&polarity_pin, GPIO_INPUT | GPIO_PULL_UP);
    read_polarity = gpio_pin_get_dt(&polarity_pin);
    //close the polarity window
    clear_CN1_6();

    return read_polarity;
}

/**
 * @brief Set the Connector 1 pin #5
 * 
 */
void set_CN1_5()
{
    gpio_pin_set_dt(&connector_pin_5, 1);
}

/**
 * @brief Clear the Connector 1 pin #5
 * 
 */
void clear_CN1_5()
{
    gpio_pin_set_dt(&connector_pin_5, 0);
}


/**
 * @brief Set the Connector 1 pin #6
 * 
 */
void set_CN1_6()
{
    gpio_pin_set_dt(&connector_pin_6, 1);
}

/**
 * @brief Clear the Connector 1 pin #6
 * 
 */
void clear_CN1_6()
{
    gpio_pin_set_dt(&connector_pin_6, 0);
}


/**
 * @brief Set the Connector 1 pin #7
 * 
 */
void set_CN1_7()
{
    gpio_pin_set_dt(&connector_pin_7, 1);
}

/**
 * @brief Clear the Connector 1 pin #7
 * 
 */
void clear_CN1_7()
{
    gpio_pin_set_dt(&connector_pin_7, 0);
}

/**
 * @brief Set the Connector 1 pin #4
 * 
 */
void set_CN1_4()
{
    gpio_pin_set_dt(&connector_pin_4, 1);
}

/**
 * @brief Clear the Connector 1 pin #4
 * 
 */
void clear_CN1_4()
{
    gpio_pin_set_dt(&connector_pin_4, 0);
}

/**
 * @brief Toggle the Connector 1 pin #6
 * 
 */
void toggle_CN_1_6()
{
   gpio_pin_toggle_dt(&connector_pin_6);
}

/**
 * @brief Toggle the connector 1 pin #4
 * 
 */
void toggle_CN_1_4()
{
    gpio_pin_toggle_dt(&connector_pin_4);
}

/**
 * @brief Toggle the Connector 1 pin #7
 * 
 */
void toggle_CN_1_7()
{
   gpio_pin_toggle_dt(&connector_pin_7);
}

/**
 * @brief Toggle the Connector 1 pin #5
 * 
 */
void toggle_CN_1_5()
{
   gpio_pin_toggle_dt(&connector_pin_5);
}

/**
 * @brief Set the imu trigger pin 
 * 
 */
void set_imu_trigger_pin()
{
    gpio_pin_set_dt(&imu_trig, 1);
}

/**
 * @brief Clear the trigger pin of IMU
 * 
 */
void clear_imu_trigger_pin()
{
    gpio_pin_set_dt(&imu_trig, 0);
}

/**
 * @brief Get the imu drdy pin status
 * 
 * @return uint8_t Current status of the DRDY pin of IMU
 */
uint8_t get_imu_drdy_pin_status()
{
    return gpio_pin_get_dt(&imu_drdy);
}

/**
 * @brief Get the temperature and presseure sensor drdy pin status
 * 
 * @return uint8_t Current status of the DRDY pin of TPS sensor
 */
uint8_t get_tps_drdy_pin_status()
{
    return gpio_pin_get_dt(&tps_drdy);
}
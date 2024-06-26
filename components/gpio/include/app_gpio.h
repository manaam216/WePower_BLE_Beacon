#ifndef __APP_GPIO__
#define __APP_GPIO__

#include <stdint.h>

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
uint8_t get_imu_drdy_pin_status();

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

#endif // __APP_GPIO__

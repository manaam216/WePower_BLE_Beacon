#ifndef __APP_GPIO__
#define __APP_GPIO__

#include <stdint.h>

/**
 * @brief Enumaeration for Polairty mode
 * 
 */
typedef enum
{
    POLARITY_NEGATIVE,
    POLARITY_POSITIVE,
    POLAIRTY_ERROR
}polairty_t;

/**
 * @brief Initialize the GPIOs of the WePower Board
 * 
 * @return  
 */
int init_we_power_board_gpios(void);

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

/**
 * @brief Read Differential Polarity
 *
 * This function reads the states of two GPIO pins to determine the 
 * polarity (positive, negative, or error) based on their states.
 * 
 * @param[out] polarity_reading_err Pointer to a variable that will 
 * receive an error flag. It is set to 0 if the reading is valid 
 * and 1 if there is an error (both pins are in the same state).
 *
 * @return uint8_t Returns one of the following:
 *         - POLARITY_POSITIVE: If the positive pin is high and the 
 *           negative pin is low.
 *         - POLARITY_NEGATIVE: If the positive pin is low and the 
 *           negative pin is high.
 *         - POLARITY_ERROR: If both pins are high or both are low, 
 *           indicating an error condition.
 */
uint8_t read_polarity(uint8_t *polarity_reading_err);

#endif // __APP_GPIO__

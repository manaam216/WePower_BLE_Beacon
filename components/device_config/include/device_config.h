#ifndef __DEVICE_CONFIG__
#define __DEVICE_CONFIG__

#include <zephyr/device.h>
#include <zephyr/devicetree.h>


/**********   IMU CONFIGURATION     ********/
#define IMU_DRDY_PIN    DT_GPIO_PIN(DT_NODELABEL(imu_drdy),gpios)
#define IMU_TRIG_PIN    DT_GPIO_PIN(DT_NODELABEL(lis_trig),gpios)

/**********  TEMP PRESSURE CONFIG  *********/
#define TPS_DRDY_PIN    DT_GPIO_PIN(DT_NODELABEL(lps_drdy),gpios)

/**********  CONNECTOR CONFIGURATION  ************/
#define POL_GPIO_PIN    DT_GPIO_PIN(DT_NODELABEL(polarity),gpios)
#define CN1_4_PIN       DT_GPIO_PIN(DT_NODELABEL(connector4),gpios)
#define CN1_5_PIN       DT_GPIO_PIN(DT_NODELABEL(connector5),gpios)
#define CN1_6_PIN       DT_GPIO_PIN(DT_NODELABEL(connector6),gpios)
#define CN1_7_PIN       DT_GPIO_PIN(DT_NODELABEL(connector7),gpios)

/*********** ERROR CONFIGURATION  ******************/
#define ERROR_PIN                   CN1_4_PIN
#define MAX_NUMBER_OF_TOGGLES_FOR_ERROR_INDICATION  100
#define UART_ERROR_DELAY_MSEC       1
#define FRAM_ERROR_DELAY_MSEC       2
#define BT_ENABLE_ERROR_DELAY_MSEC  3
#define BT_READY_ERROR_DELAY_MSEC   4 
#define ERROR_MUTEX_WAIT_TIME_MSEC        K_MSEC(500)

/******** PAYLOAD CONFIGURATION *******************/
#define PAYLOAD_DEVICE_ID_INDEX         18      // 2-least significant bytes of serial number
#define PAYLOAD_STATUS_BYTE_INDEX       20      // 0 if encrypted, 1 if AES128, 2... RFU
#define PAYLOAD_ENCRYPTION_STATUS_ENC   0
#define PAYLOAD_ENCRYPTION_STATUS_CLEAR 1
#define PAYLOAD_DATA_SIZE_BYTES         16
#define PAYLOAD_TX_REPEAT_COUNTER_INDEX 21
#define PAYLOAD_DATA_START_INDEX        2
#define PAYLOAD_FRAME_LENGTH            22
#define PAYLOAD_SERIAL_NUMBER_SIZE      2     


/******** TX REPEAT COUNTER CONFIG ***************/
#define TX_REPEAT_COUNTER_DEFAULT_VALUE     1

/******** BLUETOOTH CONFIG ***********************/
#define BLE_ADV_INTERVAL_MIN            (32) // N * 0.625. corresponds to dec. 32; 32*0.625 = 20ms
#define BLE_ADV_INTERVAL_MAX            (36) // N * 0.625. corresponds to dec. 36; 36*0.625 = 22.5ms
#define BLE_ADV_TIMEOUT                 (0)  // N * 10ms for advertiser timeout
#define BLE_ADV_EVENTS                  (1)

#endif // __DEVICE_CONFIG__

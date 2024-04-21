#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "fram.h"
#include "temp_pressure.h"
#include "accel.h"
#include "comparator.h"

#define FRAM_TEST_VALUE 33
#define FRAM_TEST_INDEX 4

LOG_MODULE_DECLARE(wepower);

typedef enum
{
    TEST_FRAM = 1,
    TEST_TEMP_PRESSURE = 2,
    TEST_ACCELEROMETER = 3,
    TEST_COMPARATOR    = 4,
}hw_tests_t;

/**
 * @brief Handle the command to run the test for FRAM
 * 
 */
static void handle_fram_test_command()
{
    uint8_t test_value = FRAM_TEST_VALUE;
    uint8_t buffer_to_read_test_value_from_fram = 0;
    uint8_t buffer_to_store_fram_current_value = 0;

    app_fram_read_field(FRAM_TEST_INDEX, &buffer_to_store_fram_current_value);
    LOG_RAW("Current value saved in local buffer %d", buffer_to_store_fram_current_value);
    app_fram_write_field(FRAM_TEST_INDEX, &test_value);
    app_fram_read_field(FRAM_TEST_INDEX, &buffer_to_read_test_value_from_fram);

    if (buffer_to_read_test_value_from_fram == test_value)
    {
        LOG_RAW("Integer test of FRAM has passed successfully");
    }
    else
    {
        LOG_RAW("Integer test of FRAM has failed");
    }
    app_fram_write_field(FRAM_TEST_INDEX, &buffer_to_store_fram_current_value); 
}

/**
 * @brief Handle the command to run the test for temperature and pressure sensor
 * 
 */
static void handle_temperature_pressure_test_command()
{
    temp_pressure_data_t temp_pressure_data = {0};
    app_temp_pressure_trigger();
    app_temp_pressure_read(&temp_pressure_data);
}

/**
 * @brief Handle the command to run the test for IMU
 * 
 */
static void handle_accelerometer_test_command()
{
    accel_data_t accel_data = {0};
    // trigger accelerometer
    accel_trigger_enable();
    app_accel_read (&accel_data);
    LOG_RAW("We shouldn't rely on actual values- since this is just raw data. Test is to just check if I2C read is successful or not");
}

/**
 * @brief Handle the command to run the test for IMU
 * 
 */
static void handle_comparator_test_command()
{
    uint8_t comparator_sample_value = init_comparator_1_vext_and_read_value();
    LOG_RAW("Read Comparator 1 value %d", comparator_sample_value);
    comparator_sample_value = get_comaprator_2_current_value();
    LOG_RAW("Read Comparator 2 value %d", comparator_sample_value);
}

/**
 * @brief Handle the command to run certain tests
 * 
 * @param received_test_number Test number to run
 */
void handle_tests_command(uint8_t received_test_number)
{
    switch (received_test_number)
    {
        case TEST_FRAM:
        {
            handle_fram_test_command();
            break;
        }
        case TEST_TEMP_PRESSURE:
        {
            handle_temperature_pressure_test_command();
            break;
        }
        case TEST_ACCELEROMETER:
        {
            handle_accelerometer_test_command();
            break;
        }
        case TEST_COMPARATOR:
        {
            handle_comparator_test_command();
            break;
        }
    default:
        break;
    }
}
#include "app_cli.h"
#include <stdlib.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/uart.h>

#include "fram.h"
#include "config_commands.h"

extern command_data_t command_data;

extern struct k_work process_command_task;

/**
 * @brief Handler for wrong format commands
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int wrong_format_handler(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh,"\r Format is incorrect \n");
    return 0;
}

/*************************************SETTER FUNCTIONS FOR FRAM FIELDS***************************/

/**
 * @brief set the event counter from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_event_counter_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = (uint8_t)atoi(argv[1]);
    command_data.data[1] = atoi(argv[1])>> (8*1);
    command_data.data[2] = atoi(argv[1])>> (8*2);
    command_data.data[3] = atoi(argv[1])>> (8*3);

    command_data.data_len = 4U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= EVENT_COUNTER_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
  
    return 0;
}

/**
 * @brief set the serial number from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_serial_number_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = (uint8_t)atoi(argv[1]);
    command_data.data[1] = atoi(argv[1])>> (8*1);
    command_data.data[2] = atoi(argv[1])>> (8*2);
    command_data.data[3] = atoi(argv[1])>> (8*3);

    command_data.data_len = 4U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= SERIAL_NUMBER_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the sensor type from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_sensor_type_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = atoi(argv[1]);
    command_data.data_len = 1U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= DEVICE_TYPE_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the packet interval from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_packet_interval_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = atoi(argv[1]);
    command_data.data_len = 1U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= ADV_INT_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the max number of packets before repeat event from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_max_number_of_packets_before_repeat_event_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = (uint8_t)atoi(argv[1]);
    command_data.data[1] = atoi(argv[1])>>8;

    command_data.data_len = 2U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= NO_OF_ADV_EVT_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the sleep time between events from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_sleep_time_between_events_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = (uint8_t)atoi(argv[1]);
    command_data.data[1] = atoi(argv[1])>> 8 ;

    command_data.data_len = 2U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= EVT_SLEEP_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the sleep time before polarity detection from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_sleep_time_before_polarity_detection_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = (uint8_t)atoi(argv[1]);
    command_data.data[1] = atoi(argv[1])>>8;

    command_data.data_len = 2U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= POL_SLEEP_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the Polarity method in FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_pol_method_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = (uint8_t)atoi(argv[1]);
    command_data.data[1] = atoi(argv[1])>>8;

    command_data.data_len = 2U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= POL_METHOD_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the ISL9122 Volts in FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_isl9122_max_volts_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0] = (uint8_t)atoi(argv[1]);
    command_data.data[1] = atoi(argv[1])>>8;

    command_data.data_len = 2U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= ISL9122_VOLTS_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the encrypted key from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_encrypted_key_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data, 0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);
    command_data.data[0]  = (uint8_t)atoi(argv[1]);
    command_data.data[1]  = (uint8_t)atoi(argv[2]);
    command_data.data[2]  = (uint8_t)atoi(argv[3]);
    command_data.data[3]  = (uint8_t)atoi(argv[4]);
    command_data.data[4]  = (uint8_t)atoi(argv[5]);
    command_data.data[5]  = (uint8_t)atoi(argv[6]);
    command_data.data[6]  = (uint8_t)atoi(argv[7]);
    command_data.data[7]  = (uint8_t)atoi(argv[8]);
    command_data.data[8]  = (uint8_t)atoi(argv[9]);
    command_data.data[9]  = (uint8_t)atoi(argv[10]);
    command_data.data[10] = (uint8_t)atoi(argv[11]);
    command_data.data[11] = (uint8_t)atoi(argv[12]);
    command_data.data[12] = (uint8_t)atoi(argv[13]);
    command_data.data[13] = (uint8_t)atoi(argv[14]);
    command_data.data[14] = (uint8_t)atoi(argv[15]);
    command_data.data[15] = (uint8_t)atoi(argv[16]);

    command_data.data_len = 16U; 

    k_work_submit(&process_command_task);
    return 0;
}

/**
 * @brief set the tx power from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_tx_power_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);

    command_data.data[0] = atoi(argv[1]);
    command_data.data_len = 1U; 

    uint64_t received_value_to_set = strtoull(argv[1], NULL, 10);

    if (received_value_to_set <= TX_POWER_MAX_VALUE)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Received Value out of bounds %lld\n", received_value_to_set);
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/**
 * @brief set the device name from FRAM
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int set_device_name_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));
    command_data.type = COMMAND_TYPE_SET;
    command_data.field_index = atoi(argv[0]);

    memcpy(command_data.data, (uint8_t*)argv[1], strlen(argv[1]));
    command_data.data_len = strlen(argv[1]);
    k_work_submit(&process_command_task);
    return 0;
}

/*********************************END OF SETTER FUNCTIONS FOR FRAM FIELDS***************************/

/********************************GETTER FUNCTIONS FOR FRAM FIELDS**********************************/

/**
 * @brief Get feild index value handler
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int get_field_index_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));

    command_data.type = COMMAND_TYPE_GET;
    command_data.field_index = atoi(argv[1]);
    if (command_data.field_index < MAX_FRAM_FIELDS)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Field index out of bounds for get command \n");
        memset(&command_data,0, sizeof(command_data));
    }

    return 0;
}

/****************************END OF GETTER FUNCTIONS FOR FRAM FIELDS****************************/

/********************************DUMP FUNCTIONS FOR FRAM FIELDS**********************************/

/**
 * @brief Dump FRAM command handler
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int dump_fram_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));

    command_data.type = COMMAND_TYPE_DUMP;
    command_data.field_index = atoi(argv[1]);

    k_work_submit(&process_command_task);
    return 0;
}

/****************************END OF DUMP FUNCTIONS FOR FRAM FIELDS****************************/

/******************************** RESET FUNCTIONS FOR FRAM FIELDS**********************************/

/**
 * @brief Reset FRAM command handler
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int reset_fram_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));

    command_data.type = COMMAND_TYPE_RESET;
    command_data.field_index = atoi(argv[1]);

    k_work_submit(&process_command_task);
    return 0;
}

/****************************END OF RESET FUNCTIONS FOR FRAM FIELDS****************************/

/******************************** PRESET FUNCTIONS FOR FRAM FIELDS**********************************/

/**
 * @brief Preset FRAM command handler
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int preset_fram_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data,0, sizeof(command_data));

    command_data.type = COMMAND_TYPE_PRESET;
    command_data.field_index = atoi(argv[1]);
    if (command_data.field_index <= PRESET_TYPE_GENERATOR)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Field index out of bounds for preset command \n");
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/****************************END OF PRESET FUNCTIONS FOR FRAM FIELDS****************************/

/******************************** CLEAR FUNCTIONS FOR FRAM FIELDS**********************************/

/**
 * @brief Clear FRAM field index command handler
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int clear_fram_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data, 0, sizeof(command_data));

    command_data.type = COMMAND_TYPE_CLEAR;
    command_data.field_index = atoi(argv[1]);

    if (command_data.field_index < MAX_FRAM_FIELDS)
    {
        k_work_submit(&process_command_task);
    }
    else
    {
        shell_print(sh,"\r Field index out of bounds for clear command \n");
        memset(&command_data,0, sizeof(command_data));
    }
    return 0;
}

/****************************END OF CLEAR FUNCTIONS FOR FRAM FIELDS****************************/

/******************************** TEST COMMAND FUNCTIONS **********************************/

/**
 * @brief Handler for receiving test commands
 * 
 * @param sh Shell object
 * @param argc Size of the arguments
 * @param argv Arguments, to be accessed as tokens
 * @return int error code
 */
static int test_command_handler(const struct shell *sh, size_t argc, char **argv)
{
    memset(&command_data, 0, sizeof(command_data));

    command_data.type = COMMAND_TYPE_TESTS;
    command_data.field_index = atoi(argv[1]);

    k_work_submit(&process_command_task);

    return 0;
}

/****************************END OF TEST COMMAND FUNCTIONS ****************************/

/**
 * @brief Initialize the command line interface for receiving commands via UART
 * 
 * @return uint8_t error code
 */
uint8_t init_command_line_interface()
{
    SHELL_STATIC_SUBCMD_SET_CREATE(set,
        SHELL_CMD(0, NULL, "set event counter.",set_event_counter_handler),
        SHELL_CMD(1, NULL, "set Serial Number.",set_serial_number_handler),
        SHELL_CMD(2, NULL, "set sensor type.",set_sensor_type_handler),
        SHELL_CMD(3, NULL, "set packet interval.",set_packet_interval_handler),
        SHELL_CMD(4, NULL, "set max number packets before repeat event.",set_max_number_of_packets_before_repeat_event_handler),
        SHELL_CMD(5, NULL, "set Sleep time between repeat events.",set_sleep_time_between_events_handler),
        SHELL_CMD(6, NULL, "set sleep time before polarity detection",set_sleep_time_before_polarity_detection_handler),
        SHELL_CMD(7, NULL, "set RFU byte.", set_isl9122_max_volts_handler),
        SHELL_CMD(8, NULL, "set RFU byte.", set_pol_method_handler),
        SHELL_CMD(9, NULL, "set Encrypted Key.",set_encrypted_key_handler),
        SHELL_CMD(10, NULL, "set TX power in 0.1 dbm.",set_tx_power_handler),
        SHELL_CMD(11, NULL, "set Device Name",set_device_name_handler),
        SHELL_SUBCMD_SET_END
    );
    SHELL_CMD_REGISTER(s, &set, "Set commands", wrong_format_handler);
    SHELL_CMD_REGISTER(g, NULL, "Get commands", get_field_index_handler);
    SHELL_CMD_REGISTER(S, &set, "Set commands", wrong_format_handler);
    SHELL_CMD_REGISTER(G, NULL, "Get commands", get_field_index_handler);
    SHELL_CMD_REGISTER(d, NULL, "Dump commands", dump_fram_handler);
    SHELL_CMD_REGISTER(D, NULL, "Dump commands", dump_fram_handler);
    SHELL_CMD_REGISTER(r, NULL, "Reset commands", reset_fram_handler);
    SHELL_CMD_REGISTER(R, NULL, "Reset commands", reset_fram_handler);
    SHELL_CMD_REGISTER(p, NULL, "Preset commands", preset_fram_handler);
    SHELL_CMD_REGISTER(P, NULL, "Preset commands", preset_fram_handler);
    SHELL_CMD_REGISTER(c, NULL, "Clear commands", clear_fram_handler);
    SHELL_CMD_REGISTER(C, NULL, "Clear commands", clear_fram_handler);
    SHELL_CMD_REGISTER(t, NULL, "Clear commands", test_command_handler);
    SHELL_CMD_REGISTER(T, NULL, "Clear commands", test_command_handler);

    #if DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_shell_uart), zephyr_cdc_acm_uart)
    const struct device *dev;
    uint32_t dtr = 0;

    dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
    if (!device_is_ready(dev) || usb_enable(NULL)) {
        return 0;
    }

    while (!dtr) {
        uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
        k_sleep(K_MSEC(100));
    }
    #endif

    return 0;
}
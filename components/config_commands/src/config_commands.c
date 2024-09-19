#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/uart.h>

#include "config_commands.h"
#include "device_config.h"

#define SIZE_OF_ENCRYPTED_KEY_STR   (3 * ENCRYPTED_KEY_NUM_BYTES) + 1
#define NUMBER_OF_BITS_IN_A_BYTE    8

#define DEFAULT_POLARITY_EVT_CTR_VALUE  0

#define PRESET0_DEFAULT_EVT_COUNTER         0
#define PRESET0_DEFAULT_SERIAL_NUM          1
#define PRESET0_DEFAULT_TYPE                0
#define PRESET0_DEFAULT_PKT_INTVAL          20
#define PRESET0_DEFAULT_EVT_MAX_PKT         254
#define PRESET0_DEFAULT_SLEEP_BTWEEN_EVTS   0
#define PRESET0_DEFAULT_SLEEP_AFTER_WAKE    0
#define PRESET0_DEFAULT_ISL9122_VOLTS       0
#define PRESET0_DEFAULT_POL_METHOD          0
#define PRESET0_DEFAULT_ENCRYPT_KEY         {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
#define PRESET0_DEFAULT_TX_POWER            80
#define PRESET0_DEFAULT_NAME                {'b','u','t','t', 'o', 'n', ' ', ' ', ' ', ' '}

#define PRESET1_DEFAULT_EVT_COUNTER         0
#define PRESET1_DEFAULT_SERIAL_NUM          1
#define PRESET1_DEFAULT_TYPE                1
#define PRESET1_DEFAULT_PKT_INTVAL          20
#define PRESET1_DEFAULT_EVT_MAX_PKT         254
#define PRESET1_DEFAULT_SLEEP_BTWEEN_EVTS   0
#define PRESET1_DEFAULT_SLEEP_AFTER_WAKE    0
#define PRESET1_DEFAULT_ISL9122_VOLTS       0
#define PRESET1_DEFAULT_POL_METHOD          0
#define PRESET1_DEFAULT_ENCRYPT_KEY         {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
#define PRESET1_DEFAULT_TX_POWER            80
#define PRESET1_DEFAULT_NAME                {'b','u','t','t', 'o', 'n', ' ', ' ', ' ', ' '}

#define PRESET2_DEFAULT_EVT_COUNTER         0
#define PRESET2_DEFAULT_SERIAL_NUM          1
#define PRESET2_DEFAULT_TYPE                2
#define PRESET2_DEFAULT_PKT_INTVAL          20
#define PRESET2_DEFAULT_EVT_MAX_PKT         10
#define PRESET2_DEFAULT_SLEEP_BTWEEN_EVTS   9800
#define PRESET2_DEFAULT_SLEEP_AFTER_WAKE    0
#define PRESET2_DEFAULT_ISL9122_VOLTS       0
#define PRESET2_DEFAULT_POL_METHOD          0
#define PRESET2_DEFAULT_ENCRYPT_KEY         {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
#define PRESET2_DEFAULT_TX_POWER            80
#define PRESET2_DEFAULT_NAME                {'v','i','b','r', 'a', 't', 'i', 'o', 'n', ' '}

#define PRESET3_DEFAULT_EVT_COUNTER         0
#define PRESET3_DEFAULT_SERIAL_NUM          0
#define PRESET3_DEFAULT_TYPE                3
#define PRESET3_DEFAULT_PKT_INTVAL          20
#define PRESET3_DEFAULT_EVT_MAX_PKT         254
#define PRESET3_DEFAULT_SLEEP_BTWEEN_EVTS   0
#define PRESET3_DEFAULT_SLEEP_AFTER_WAKE    2
#define PRESET3_DEFAULT_ISL9122_VOLTS       0
#define PRESET3_DEFAULT_POL_METHOD          0
#define PRESET3_DEFAULT_ENCRYPT_KEY         {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
#define PRESET3_DEFAULT_TX_POWER            80
#define PRESET3_DEFAULT_NAME                {'o','n','-','o', 'f', 'f', ' ', 's', 'w', ' '}

#define PRESET4_DEFAULT_EVT_COUNTER         0
#define PRESET4_DEFAULT_SERIAL_NUM          0
#define PRESET4_DEFAULT_TYPE                4
#define PRESET4_DEFAULT_PKT_INTVAL          20
#define PRESET4_DEFAULT_EVT_MAX_PKT         254
#define PRESET4_DEFAULT_SLEEP_BTWEEN_EVTS   0
#define PRESET4_DEFAULT_SLEEP_AFTER_WAKE    0
#define PRESET4_DEFAULT_ISL9122_VOLTS       0
#define PRESET4_DEFAULT_POL_METHOD          0
#define PRESET4_DEFAULT_ENCRYPT_KEY         {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
#define PRESET4_DEFAULT_TX_POWER            80
#define PRESET4_DEFAULT_NAME                {'l','e','a','k', ' ', 's', 'e', 'n', ' ', ' '}

#define COMMAND_TYPE_TO_STR(x)  (x == COMMAND_TYPE_SET)?    "SET":\
                                (x == COMMAND_TYPE_GET)?    "GET":\
                                (x == COMMAND_TYPE_DUMP)?   "DUMP":\
                                (x == COMMAND_TYPE_RESET)?  "RESET":\
                                (x == COMMAND_TYPE_PRESET)? "PRESET":\
                                (x == COMMAND_TYPE_CLEAR)?  "CLEAR":\
                                "UNKNOWN CMD RECEIVED"

LOG_MODULE_DECLARE(wepower);

// Buffer Contatining Encrypted Key
extern uint8_t ecb_key[ENCRYPTED_KEY_NUM_BYTES];

// variable to store the FRAM data block
fram_data_t fram_data = {0};

// BUffer Containing received Command over UART
command_data_t command_data = {0};

// Device Tree Node of th4e UART
static const struct device *const dev_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

// Work Task to process UART commands
extern struct k_work process_command_task;

const fram_info_t FRAM_INFO[MAX_FRAM_FIELDS] = 
{
    {"EVENT Counter",           DATA_NUMBER, FRAM_COUNTER_NUM_BYTES, EVENT_COUNTER_MIN_VALUE, EVENT_COUNTER_MAX_VALUE, EVENT_COUNTER_DEFAULT_VALUE},
    {"SERIAL NUMBER",           DATA_NUMBER, SER_NUM_BYTES,          SERIAL_NUMBER_MIN_VALUE, SERIAL_NUMBER_MAX_VALUE, SERIAL_NUMBER_DEFAULT_VALUE},
    {"TYPE",                    DATA_NUMBER, TYPE_NUM_BYTES,         DEVCIE_TYPE_MIN_VALUE,   DEVICE_TYPE_MAX_VALUE,   DEVICE_TYPE_DEFAULT_VALUE},
    {"REPEAT INTERVAL",         DATA_NUMBER, EV_INT_NUM_BYTES,       ADV_EVT_INT_MIN_VALUE,   ADV_INT_MAX_VALUE,       ADV_INT_DEFAULT_VALUE},  // advertising interval within an event
    {"EVENT MAXIMUM REPEATS",   DATA_NUMBER, EV_MAX_NUM_BYTES,       NO_OF_ADV_EVT_MIN_VALUE, NO_OF_ADV_EVT_MAX_VALUE, NO_OF_ADV_EVT_DEFAULT_VALUE}, // max advertising repeats within an event
    {"SLEEP BETWEEN EVENTS",    DATA_NUMBER, EV_SLP_NUM_BYTES,       EVT_SLEEP_MIN_VALUE,     EVT_SLEEP_MAX_VALUE,     EVT_SLEEP_DEFAULT_VALUE}, // sleep time before next event, 0 just wastes rest of power.
    {"SLEEP BEFORE POLARITY",   DATA_NUMBER, IN_SLP_NUM_BYTES,       POL_SLEEP_MIN_VALUE,     POL_SLEEP_MAX_VALUE,     POL_SLEEP_DEFAULT_VALUE}, 
    {"unused",                  DATA_NUMBER, ISL9122_NUM_BYTES,      ISL9122_VOLTS_MIN_VALUE, ISL9122_VOLTS_MAX_VALUE, ISL9122_VOLTS_DEFAULT_VALUE},
    {"unused",                  DATA_NUMBER, POL_MET_NUM_BYTES,      POL_METHOD_MIN_VALUE, POL_METHOD_MAX_VALUE, POL_METHOD_DEFAULT_VALUE},
    {"ENCRYPTED KEY",       DATA_BYTE_ARRAY, ENCRYPTED_KEY_NUM_BYTES, 0, 0,0}, // Since this is a byte array, mix max values do not matter
    {"TX dBm 10 (R.F.U.)",      DATA_NUMBER, TX_DBM_NUM_BYTES,       TX_POWER_MIN_VALUE, TX_POWER_MAX_VALUE, TX_POWER_DEFAULT_VALUE},
    {"Device NAME",             DATA_STRING, NAME_NUM_BYTES,         0, 0,0}, // Since this is astring, max and min values do not matter
    {"NEG EVT CTR",             DATA_NUMBER, NEG_EVT_CTR_BYTES, POLARITY_COUNTERS_MIN_VALUE, POLARITY_COUNTERS_MAX_VALUE, POLARITY_COUNTERS_DEFAULT_VALUE},
    {"POS EVT CTR",             DATA_NUMBER, POS_EVT_CTR_BYTES, POLARITY_COUNTERS_MIN_VALUE, POLARITY_COUNTERS_MAX_VALUE, POLARITY_COUNTERS_DEFAULT_VALUE},
};

/**
 * @brief Preset 0 will be used for button type 0
 * 
 */
static fram_data_t Preset0 = 
{
    PRESET0_DEFAULT_EVT_COUNTER,
    PRESET0_DEFAULT_SERIAL_NUM,
    PRESET0_DEFAULT_TYPE,
    PRESET0_DEFAULT_PKT_INTVAL,
    PRESET1_DEFAULT_EVT_MAX_PKT,   
    PRESET0_DEFAULT_SLEEP_BTWEEN_EVTS,    
    PRESET0_DEFAULT_SLEEP_AFTER_WAKE,
    PRESET0_DEFAULT_ISL9122_VOLTS,
    PRESET0_DEFAULT_POL_METHOD,
    PRESET0_DEFAULT_ENCRYPT_KEY,
    PRESET0_DEFAULT_TX_POWER,
    PRESET0_DEFAULT_NAME,
    DEFAULT_POLARITY_EVT_CTR_VALUE,
    DEFAULT_POLARITY_EVT_CTR_VALUE
};

/**
 * @brief Preset 1 will be used for button type 2
 * 
 */
static fram_data_t Preset1 = 
{
    PRESET1_DEFAULT_EVT_COUNTER,
    PRESET1_DEFAULT_SERIAL_NUM,
    PRESET1_DEFAULT_TYPE,
    PRESET1_DEFAULT_PKT_INTVAL,
    PRESET1_DEFAULT_EVT_MAX_PKT,   
    PRESET1_DEFAULT_SLEEP_BTWEEN_EVTS,    
    PRESET1_DEFAULT_SLEEP_AFTER_WAKE,
    PRESET1_DEFAULT_ISL9122_VOLTS,
    PRESET1_DEFAULT_POL_METHOD,
    PRESET1_DEFAULT_ENCRYPT_KEY,
    PRESET1_DEFAULT_TX_POWER,
    PRESET1_DEFAULT_NAME,
    DEFAULT_POLARITY_EVT_CTR_VALUE,
    DEFAULT_POLARITY_EVT_CTR_VALUE
};

 /**
  * @brief Preset 2 will be used for button type 2 (periodic events every 10s from cont. harvest)
  * 
  */
static fram_data_t Preset2 = 
{
    PRESET2_DEFAULT_EVT_COUNTER,
    PRESET2_DEFAULT_SERIAL_NUM,
    PRESET2_DEFAULT_TYPE,
    PRESET2_DEFAULT_PKT_INTVAL,
    PRESET2_DEFAULT_EVT_MAX_PKT,   
    PRESET2_DEFAULT_SLEEP_BTWEEN_EVTS,    
    PRESET2_DEFAULT_SLEEP_AFTER_WAKE,
    PRESET2_DEFAULT_ISL9122_VOLTS,
    PRESET2_DEFAULT_POL_METHOD,
    PRESET2_DEFAULT_ENCRYPT_KEY,
    PRESET2_DEFAULT_TX_POWER,
    PRESET2_DEFAULT_NAME,
    DEFAULT_POLARITY_EVT_CTR_VALUE,
    DEFAULT_POLARITY_EVT_CTR_VALUE
};

 /**
  * @brief Preset 3 will be used for limit/slide polarized switch type 3
  * 
  */
static fram_data_t Preset3 = 
{
    PRESET3_DEFAULT_EVT_COUNTER,
    PRESET3_DEFAULT_SERIAL_NUM,
    PRESET3_DEFAULT_TYPE,
    PRESET3_DEFAULT_PKT_INTVAL,
    PRESET3_DEFAULT_EVT_MAX_PKT,   
    PRESET3_DEFAULT_SLEEP_BTWEEN_EVTS,    
    PRESET3_DEFAULT_SLEEP_AFTER_WAKE,
    PRESET3_DEFAULT_ISL9122_VOLTS,
    PRESET3_DEFAULT_POL_METHOD,
    PRESET3_DEFAULT_ENCRYPT_KEY,
    PRESET3_DEFAULT_TX_POWER,
    PRESET3_DEFAULT_NAME,
    DEFAULT_POLARITY_EVT_CTR_VALUE,
    DEFAULT_POLARITY_EVT_CTR_VALUE
};

 /**
  * @brief Preset 4 will be used for release type genrator type 4
  * 
  */
static fram_data_t Preset4 = 
{
    PRESET4_DEFAULT_EVT_COUNTER,
    PRESET4_DEFAULT_SERIAL_NUM,
    PRESET4_DEFAULT_TYPE,
    PRESET4_DEFAULT_PKT_INTVAL,
    PRESET4_DEFAULT_EVT_MAX_PKT,   
    PRESET4_DEFAULT_SLEEP_BTWEEN_EVTS,    
    PRESET4_DEFAULT_SLEEP_AFTER_WAKE,
    PRESET4_DEFAULT_ISL9122_VOLTS,
    PRESET4_DEFAULT_POL_METHOD,
    PRESET4_DEFAULT_ENCRYPT_KEY,
    PRESET4_DEFAULT_TX_POWER,
    PRESET4_DEFAULT_NAME,
    DEFAULT_POLARITY_EVT_CTR_VALUE,
    DEFAULT_POLARITY_EVT_CTR_VALUE
};

/**
 * @brief Function to initialize the UART functionality
 * 
 * @return int Error code
 */
int init_uart(void)
{
	if (dev_uart == NULL) {
		LOG_ERR("Failed to get UART binding");
		return -1;
	}

    if (!device_is_ready(dev_uart)) {
		LOG_ERR("UART device not found!");
		return -2;
	}
   
    LOG_INF("UART is initiated.\n");

    return 0;
}

/**
 * @brief Disbale UART Peripheral explictly 
 * 
 */
void disable_uart(void)
{
    pm_device_action_run(dev_uart, PM_DEVICE_ACTION_SUSPEND);
}

/**
 * @brief Preset the FRAM by the type of the sensor
 * 
 * @param preset_type uint8_t preset type to apply to the device
 */
void preset_fram_by_type(uint8_t preset_type)
{
    LOG_RAW("Presetting as type %d\n", preset_type);
    switch (preset_type)
    {
        case PRESET_TYPE_BUTTON_0:
            app_fram_write_field(EV_CTR, (uint8_t*) &Preset0.event_counter);
            app_fram_write_field(SER_NUM, (uint8_t*) &Preset0.serial_number);
            app_fram_write_field(TYPE, (uint8_t*) &Preset0.type);
            app_fram_write_field(EV_INT, (uint8_t*) &Preset0.packet_interval);
            app_fram_write_field(EV_MAX, (uint8_t*) &Preset0.event_max_packets);
            app_fram_write_field(EV_SLP, (uint8_t*) &Preset0.sleep_between_events);
            app_fram_write_field(IN_SLP, (uint8_t*) &Preset0.sleep_after_wake);
            app_fram_write_field(ISL9122, (uint8_t*) &Preset0.u8_voltsISL9122);
            app_fram_write_field(POL_MET, (uint8_t*) &Preset0.u8_POLmethod);
            app_fram_write_field(ENCRYPTED_KEY, (uint8_t*) &Preset0.encrypted_key);
            app_fram_write_field(TX_DBM, (uint8_t*) &Preset0.tx_dbm_10);
            app_fram_write_field(NAME, (uint8_t*) &Preset0.cName);
            app_fram_write_field(NEG_EVT_CTR, (uint8_t*) &Preset0.negative_events_counter);
            app_fram_write_field(POS_EVT_CTR, (uint8_t*) &Preset0.positive_events_counter);
            break;
            
        case PRESET_TYPE_BUTTON_1:
            app_fram_write_field(EV_CTR, (uint8_t*) &Preset1.event_counter);
            app_fram_write_field(SER_NUM, (uint8_t*) &Preset1.serial_number);
            app_fram_write_field(TYPE, (uint8_t*) &Preset1.type);
            app_fram_write_field(EV_INT, (uint8_t*) &Preset1.packet_interval);
            app_fram_write_field(EV_MAX, (uint8_t*) &Preset1.event_max_packets);
            app_fram_write_field(EV_SLP, (uint8_t*) &Preset1.sleep_between_events);
            app_fram_write_field(IN_SLP, (uint8_t*) &Preset1.sleep_after_wake);
            app_fram_write_field(ISL9122, (uint8_t*) &Preset1.u8_voltsISL9122);
            app_fram_write_field(POL_MET, (uint8_t*) &Preset1.u8_POLmethod);
            app_fram_write_field(ENCRYPTED_KEY, (uint8_t*) &Preset1.encrypted_key);
            app_fram_write_field(TX_DBM, (uint8_t*) &Preset1.tx_dbm_10);
            app_fram_write_field(NAME, (uint8_t*) &Preset1.cName);
            app_fram_write_field(NEG_EVT_CTR, (uint8_t*) &Preset1.negative_events_counter);
            app_fram_write_field(POS_EVT_CTR, (uint8_t*) &Preset1.positive_events_counter);
            break;
            
        case PRESET_TYPE_VIB_SENS:
            app_fram_write_field(EV_CTR, (uint8_t*) &Preset2.event_counter);
            app_fram_write_field(SER_NUM, (uint8_t*) &Preset2.serial_number);
            app_fram_write_field(TYPE, (uint8_t*) &Preset2.type);
            app_fram_write_field(EV_INT, (uint8_t*) &Preset2.packet_interval);
            app_fram_write_field(EV_MAX, (uint8_t*) &Preset2.event_max_packets);
            app_fram_write_field(EV_SLP, (uint8_t*) &Preset2.sleep_between_events);
            app_fram_write_field(IN_SLP, (uint8_t*) &Preset2.sleep_after_wake);
            app_fram_write_field(ISL9122, (uint8_t*) &Preset2.u8_voltsISL9122);
            app_fram_write_field(POL_MET, (uint8_t*) &Preset2.u8_POLmethod);
            app_fram_write_field(ENCRYPTED_KEY, (uint8_t*) &Preset2.encrypted_key);
            app_fram_write_field(TX_DBM, (uint8_t*) &Preset2.tx_dbm_10);
            app_fram_write_field(NAME, (uint8_t*) &Preset2.cName);
            app_fram_write_field(NEG_EVT_CTR, (uint8_t*) &Preset2.negative_events_counter);
            app_fram_write_field(POS_EVT_CTR, (uint8_t*) &Preset2.positive_events_counter);
            break;
            
        case PRESET_TYPE_ON_OFF_SW:
            app_fram_write_field(EV_CTR, (uint8_t*) &Preset3.event_counter);
            app_fram_write_field(SER_NUM, (uint8_t*) &Preset3.serial_number);
            app_fram_write_field(TYPE, (uint8_t*) &Preset3.type);
            app_fram_write_field(EV_INT, (uint8_t*) &Preset3.packet_interval);
            app_fram_write_field(EV_MAX, (uint8_t*) &Preset3.event_max_packets);
            app_fram_write_field(EV_SLP, (uint8_t*) &Preset3.sleep_between_events);
            app_fram_write_field(IN_SLP, (uint8_t*) &Preset3.sleep_after_wake);
            app_fram_write_field(ISL9122, (uint8_t*) &Preset3.u8_voltsISL9122);
            app_fram_write_field(POL_MET, (uint8_t*) &Preset3.u8_POLmethod);
            app_fram_write_field(ENCRYPTED_KEY, (uint8_t*) &Preset3.encrypted_key);
            app_fram_write_field(TX_DBM, (uint8_t*) &Preset3.tx_dbm_10);
            app_fram_write_field(NAME, (uint8_t*) &Preset3.cName);
            app_fram_write_field(NEG_EVT_CTR, (uint8_t*) &Preset3.negative_events_counter);
            app_fram_write_field(POS_EVT_CTR, (uint8_t*) &Preset3.positive_events_counter);
            break;
            
        case PRESET_TYPE_GENERATOR:
            app_fram_write_field(EV_CTR, (uint8_t*) &Preset4.event_counter);
            app_fram_write_field(SER_NUM, (uint8_t*) &Preset4.serial_number);
            app_fram_write_field(TYPE, (uint8_t*) &Preset4.type);
            app_fram_write_field(EV_INT, (uint8_t*) &Preset4.packet_interval);
            app_fram_write_field(EV_MAX, (uint8_t*) &Preset4.event_max_packets);
            app_fram_write_field(EV_SLP, (uint8_t*) &Preset4.sleep_between_events);
            app_fram_write_field(IN_SLP, (uint8_t*) &Preset4.sleep_after_wake);
            app_fram_write_field(ISL9122, (uint8_t*) &Preset4.u8_voltsISL9122);
            app_fram_write_field(POL_MET, (uint8_t*) &Preset4.u8_POLmethod);
            app_fram_write_field(ENCRYPTED_KEY, (uint8_t*) &Preset4.encrypted_key);
            app_fram_write_field(TX_DBM, (uint8_t*) &Preset4.tx_dbm_10);
            app_fram_write_field(NAME, (uint8_t*) &Preset4.cName);
            app_fram_write_field(NEG_EVT_CTR, (uint8_t*) &Preset4.negative_events_counter);
            app_fram_write_field(POS_EVT_CTR, (uint8_t*) &Preset4.positive_events_counter);
            break;
        default:
            break; 
    }
}

/**
 * @brief Function to dump content to FRAM
 * 
 * @param print Flag to indicate if printing should be done
 * @return int32_t error code
 */
int32_t dump_fram(uint8_t print)
{    
    uint32_t ret = app_fram_read_data(&fram_data);
    if((ret == FRAM_SUCCESS) && print)
        {
            // Success! Add it to the payload
            LOG_RAW("%s",">> ------- Reading FRAM Data -------");

		    LOG_RAW("FRAM Index [0]->Event Counter: %d", fram_data.event_counter);            
            LOG_RAW("FRAM Index [1]->Serial Number: %d", fram_data.serial_number);
            LOG_RAW("FRAM Index [2]->Device Type: %d", fram_data.type);
		    LOG_RAW("FRAM Index [3]->Packet Repeat Interval: %d ms", fram_data.packet_interval);
		    LOG_RAW("FRAM Index [4]->Maximum Packets per Event: %d", fram_data.event_max_packets);
		    LOG_RAW("FRAM Index [5]->Sleep Between Events: %d", fram_data.sleep_between_events);
		    LOG_RAW("FRAM Index [6]->Sleep Before Testing Polarity: %d", fram_data.sleep_after_wake);
		    LOG_RAW("FRAM Index [7]->Reserved Byte: %d", fram_data.u8_voltsISL9122);
		    LOG_RAW("FRAM Index [8]->Reserved Byte: %d", fram_data.u8_POLmethod);
            LOG_RAW("FRAM Index [9]->Reserved for Programmable Encrypted Key:  %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
                    fram_data.encrypted_key[0], fram_data.encrypted_key[1], fram_data.encrypted_key[2], fram_data.encrypted_key[3], 
                    fram_data.encrypted_key[4], fram_data.encrypted_key[5], fram_data.encrypted_key[6], fram_data.encrypted_key[7], 
                    fram_data.encrypted_key[8], fram_data.encrypted_key[9], fram_data.encrypted_key[10], fram_data.encrypted_key[11], 
                    fram_data.encrypted_key[12], fram_data.encrypted_key[13], fram_data.encrypted_key[14], fram_data.encrypted_key[15]);
            LOG_RAW("FRAM Index [10]->Reserved for adjustable TX dBm 10: %d", fram_data.tx_dbm_10);
		    LOG_RAW("FRAM Index [11]->cName: %s", fram_data.cName);
            LOG_RAW("FRAM Index [12]->negative_events_counter: %d", fram_data.negative_events_counter);
            LOG_RAW("FRAM Index [13]->positive_events_counter: %d", fram_data.positive_events_counter);
        }

    if (ret != FRAM_SUCCESS) 
    {
		LOG_ERR("FRAM error!");
    }
    return ret;
}

/**
 * @brief Handle "get" command from the user
 * 
 */
static void handle_get_command()
{
    uint8_t encrypted_key_str[SIZE_OF_ENCRYPTED_KEY_STR] = {0};
    uint32_t field_data = 0;
    if ( app_fram_read_field(command_data.field_index, command_data.data) == FRAM_SUCCESS)
    {
        switch (FRAM_INFO[command_data.field_index].type)
        {
        case DATA_NUMBER:
            // Converting Data to integer type
            memcpy(&field_data, command_data.data, FRAM_INFO[command_data.field_index].field_length);

            LOG_INF("FRAM field [%d] %s, length, %d, value is %d.\n", 
                    command_data.field_index,
                    FRAM_INFO[command_data.field_index].name, 
                    FRAM_INFO[command_data.field_index].field_length, 
                    field_data);
            break;

        case DATA_STRING:
            LOG_INF("FRAM field [%d] %s, length, %d, value is %s.\n", 
                    command_data.field_index,
                    FRAM_INFO[command_data.field_index].name, 
                    FRAM_INFO[command_data.field_index].field_length, 
                    command_data.data);
            break;

        case DATA_BYTE_ARRAY:
            for (uint8_t encrypted_key_str_idx = 0; 
                 encrypted_key_str_idx < ENCRYPTED_KEY_NUM_BYTES; 
                 encrypted_key_str_idx++)
            {
                sprintf(&encrypted_key_str[(encrypted_key_str_idx * 3)], "%02X ", command_data.data[encrypted_key_str_idx]);
            }
            // Adding fullstop at the end, used for pretty printing
            encrypted_key_str[(3*ENCRYPTED_KEY_NUM_BYTES - 1)] = '.';
            LOG_INF("FRAM field [%d] %s, length, %d, value is 0x%s\n", 
                    command_data.field_index,
                    FRAM_INFO[command_data.field_index].name, 
                    FRAM_INFO[command_data.field_index].field_length, 
                    encrypted_key_str);
            break;

        default:
            // Do nothing
            break;
        }
    }
    else
    {
        LOG_INF("Unable to read the FRAM, failed to handle get command");
    }
}

/**
 * @brief Handle "clear" command from the user
 * 
 */
static void handle_clear_command()
{
    uint32_t field_data = 0;

    switch (FRAM_INFO[command_data.field_index].type)
    {
    case DATA_NUMBER:
        field_data = 0;
        if (app_fram_write_field(command_data.field_index, (uint8_t*)&field_data) == FRAM_SUCCESS)
        {
            LOG_INF("FRAM field [%d] %s, Value is %u.\n", 
                    command_data.field_index, 
                    FRAM_INFO[command_data.field_index].name, 
                    field_data);
        }
        else
        {
            LOG_INF("Access to FRAM failed to clear numeric data");
        }
        break;

    case DATA_STRING:
        memset(command_data.data, ' ', FRAM_INFO[command_data.field_index].field_length);
        if(app_fram_write_field(command_data.field_index, command_data.data) == FRAM_SUCCESS)
        {
            LOG_INF("FRAM field [%d] %s, Value is %s. \n", 
                    command_data.field_index,
                    FRAM_INFO[command_data.field_index].name, 
                    command_data.data);
        }
        else
        {
            LOG_INF("Access to FRAM failed to clear string data");
        }
        break;

    case DATA_BYTE_ARRAY:
        memset(command_data.data, 0, FRAM_INFO[command_data.field_index].field_length);
        if(app_fram_write_field(command_data.field_index, command_data.data) == FRAM_SUCCESS)
        {
            LOG_INF( "FRAM field [%d] %s, Value is %s.\n", command_data.field_index, FRAM_INFO[command_data.field_index].name, command_data.data);
        }
        else
        {
            LOG_INF("Access to FRAM failed to clear byte array");
        }
        break;

    default:
        break;
    }
}

/**
 * @brief Handle "dump" command from the user
 * 
 */
static void handle_dump_command()
{
    dump_fram(true);
}

/**
 * @brief Handle "reset" command from the user
 * 
 */
static void handle_reset_commmand()
{
    uint32_t field_data = 0;

    for (uint8_t fram_info_arr_idx = EV_CTR; fram_info_arr_idx < MAX_FRAM_FIELDS; fram_info_arr_idx++)
    {
        command_data.field_index = fram_info_arr_idx;
        switch (FRAM_INFO[fram_info_arr_idx].type)
        {
        case DATA_NUMBER:
            field_data = FRAM_INFO[fram_info_arr_idx].default_value;
            if (app_fram_write_field(command_data.field_index, (uint8_t*)&field_data) == FRAM_SUCCESS)
            {
                LOG_INF( "FRAM field [%d] %s, Value is %u ", command_data.field_index, FRAM_INFO[command_data.field_index].name, field_data);
            }
            else 
            {
                LOG_INF( "Access to FRAM failed.\n");
            }
            break;

        case DATA_STRING:
            //Cleaning Command.data with Zeros
            memset(command_data.data, 0x00, 100);
            //Assigning Space (' ') to the Field Name
            memset(command_data.data, ' ', FRAM_INFO[command_data.field_index].field_length);
            if(app_fram_write_field(command_data.field_index, command_data.data) == FRAM_SUCCESS)
            {
                LOG_INF( "FRAM field [%d] %s, Value is %s", command_data.field_index, FRAM_INFO[command_data.field_index].name, command_data.data);
            }
            else
            {
                LOG_INF("Access to FRAM failed.\n");
            }
            break;

        case DATA_BYTE_ARRAY:
            memset(command_data.data, 0x00, FRAM_INFO[command_data.field_index].field_length);
            for (uint8_t default_encrypted_key_idx = 0; 
                default_encrypted_key_idx < ENCRYPTED_KEY_NUM_BYTES; 
                default_encrypted_key_idx++)
            {
                command_data.data[default_encrypted_key_idx] =  ecb_key[default_encrypted_key_idx];
            }

            if(app_fram_write_field(command_data.field_index, command_data.data) == FRAM_SUCCESS)
            {
                LOG_INF("FRAM field [%d] %s, Value is %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
                        command_data.field_index, 
                        FRAM_INFO[command_data.field_index].name,
                        command_data.data[0],
                        command_data.data[1],
                        command_data.data[2],
                        command_data.data[3],
                        command_data.data[4],
                        command_data.data[5],
                        command_data.data[6],
                        command_data.data[7],
                        command_data.data[8],
                        command_data.data[9],
                        command_data.data[10],
                        command_data.data[11],
                        command_data.data[12],
                        command_data.data[13],
                        command_data.data[14],
                        command_data.data[15] );
            }
            else
            {
                LOG_INF("Access to FRAM failed.\n");
            }
            break;

        default:
            break;
        }
    }
}

/**
 * @brief Handle "set" command from the user
 * 
 */
static void handle_set_command()
{
    uint8_t encrypted_key_str[SIZE_OF_ENCRYPTED_KEY_STR] = {0};
    uint32_t field_data = 0;

    switch (FRAM_INFO[command_data.field_index].type)
    {
    case DATA_NUMBER:
        for (uint8_t command_data_idx = 0; 
             command_data_idx < command_data.data_len; 
             command_data_idx++)
        {
            field_data = field_data + (command_data.data[command_data_idx] << (NUMBER_OF_BITS_IN_A_BYTE * command_data_idx));
        }
        LOG_INF(" Numeric Data: %u\n", field_data);
        // Check if value is in bound, if not then read the current value from FRAM
        if(field_data < FRAM_INFO[command_data.field_index].min_value)
        {
            if (app_fram_read_field(command_data.field_index, (uint8_t*)&field_data) == FRAM_SUCCESS)
            {
                LOG_INF( "FRAM field [%d] %s, Min limit %u, Value is %u.\n", command_data.field_index,
                    FRAM_INFO[command_data.field_index].name, FRAM_INFO[command_data.field_index].min_value, field_data);
            }
            else
            {
                LOG_INF("Access to FRAM failed.\n");
            }
        }
        else if (field_data > FRAM_INFO[command_data.field_index].max_value)
        {
            if (app_fram_read_field(command_data.field_index, (uint8_t*)&field_data) == FRAM_SUCCESS)
            {
                LOG_INF("FRAM field [%d] %s, Max limit %u exceeded, Value is %u.\n", command_data.field_index,
                    FRAM_INFO[command_data.field_index].name, FRAM_INFO[command_data.field_index].max_value, field_data);
            } 
            else
            {
                LOG_INF("Access to FRAM failed.\n");
            }
        }
        else
        {
            if (app_fram_write_field(command_data.field_index, (uint8_t*)&field_data) == FRAM_SUCCESS)
            {
                LOG_INF("FRAM field [%d] %s, Value is %u.\n", command_data.field_index, FRAM_INFO[command_data.field_index].name, field_data);
            }
            else 
            {
                LOG_INF("Access to FRAM failed.\n");
            }
        } 
        break;

    case DATA_STRING:
        // Check if value is in bound, If not then read the current value
        if (command_data.data_len > FRAM_INFO[command_data.field_index].field_length)
        {
            if (app_fram_read_field(command_data.field_index, (uint8_t*)&command_data.data) == FRAM_SUCCESS)
            {
                LOG_INF("Number of characters exceeds maximum length of 10 bytes. FRAM field %s, length exceeded, Value is %s.\n", 
                    FRAM_INFO[command_data.field_index].name, command_data.data);
            }
            else
            {
                LOG_INF("Access to FRAM failed.\n");
            }
        }
        else
        {
            if(app_fram_write_field(command_data.field_index, command_data.data) == FRAM_SUCCESS)
            {
                LOG_INF("Fram field [%d] %s, length %d, value is %s.\n", command_data.field_index,
                        FRAM_INFO[command_data.field_index].name, 
                        FRAM_INFO[command_data.field_index].field_length, 
                        command_data.data);
            }
            else 
            {
                LOG_INF("Access to FRAM failed.\n");
            }
        }
        break;

    case DATA_BYTE_ARRAY:
        if(app_fram_write_field(command_data.field_index, command_data.data) == FRAM_SUCCESS)
        {
            for (uint8_t encrypted_key_str_idx = 0; 
                 encrypted_key_str_idx < ENCRYPTED_KEY_NUM_BYTES; 
                 encrypted_key_str_idx++)
            {
                sprintf(&encrypted_key_str[(encrypted_key_str_idx * 3)], "%02X ", command_data.data[encrypted_key_str_idx]);
            }
            // Adding fullstop at the end, used for pretty printing
            encrypted_key_str[(3*ENCRYPTED_KEY_NUM_BYTES - 1)] = '.';
            LOG_INF("FRAM field [%d] %s, length, %d, value is 0x%s\n", 
                    command_data.field_index,
                    FRAM_INFO[command_data.field_index].name, 
                    FRAM_INFO[command_data.field_index].field_length, 
                    encrypted_key_str);     
        } 
        else 
        {
            LOG_INF("Access to FRAM failed.\n");
        }
        break;

    default:
        break;
    }
}

/**
 * @brief Handle Preset command from the user
 * 
 */
static void handle_preset_commmand()
{
    preset_fram_by_type(command_data.field_index);
    dump_fram(1);
}

/**
 * @brief Process Command function
 * 
 * @param work Work Item to process
 */
void process_command_fn(struct k_work *work)
{           
    LOG_INF("Parsed Command is Type: %s, field: %d, data: %s\n", COMMAND_TYPE_TO_STR(command_data.type), command_data.field_index, command_data.data);

    switch (command_data.type)
    {
    case COMMAND_TYPE_GET:
        LOG_INF("Get: G/g Command Received with Field Index: %d",command_data.field_index);
        handle_get_command();
        break;
    case COMMAND_TYPE_SET:
        LOG_INF("Set: S/s Command Received with Field Index: %d  Value: %s",command_data.field_index,command_data.data);
        handle_set_command(); 
        break;
    case COMMAND_TYPE_CLEAR:
        LOG_INF("Clear: C/c Command Received with Field Index: %d",command_data.field_index);
        handle_clear_command();
        break;
    case COMMAND_TYPE_RESET:
        LOG_INF( "Reset All Data: R/r Command Received");
        handle_reset_commmand(); 
    case COMMAND_TYPE_DUMP:
        LOG_INF( "Dump: D/d Command Received");
        handle_dump_command();
        break;
    case COMMAND_TYPE_PRESET:
        LOG_INF( "Preset: P/p Command Received for type %d\n", command_data.field_index);
        handle_preset_commmand();
        break;
    case COMMAND_TYPE_TESTS:
        LOG_INF( "Running the test command received");
        handle_tests_command(command_data.field_index);
        break;
    default:
        break;
    }
    
    memset(&command_data,0, sizeof(command_data));
    return;
}
#ifndef __FRAM__
#define __FRAM__

#include <stdint.h>

#define FRAM_ERROR  -1              // FRAM Error Code
#define FRAM_SUCCESS 0              // FRAM Success Code

#define FRAM_COUNTER_ADDR 			(0x0000)
#define FRAM_COUNTER_NUM_BYTES 		(4)
#define SER_NUM_ADDR 				(FRAM_COUNTER_ADDR+FRAM_COUNTER_NUM_BYTES)
#define SER_NUM_BYTES 				(4)
#define TYPE_ADDR					(SER_NUM_ADDR+SER_NUM_BYTES)
#define TYPE_NUM_BYTES 				(1)
#define EV_INT_ADDR					(TYPE_ADDR+TYPE_NUM_BYTES)
#define EV_INT_NUM_BYTES			(1)
#define EV_MAX_ADDR					(EV_INT_ADDR+EV_INT_NUM_BYTES)
#define EV_MAX_NUM_BYTES			(2)
#define EV_SLP_ADDR					(EV_MAX_ADDR+EV_MAX_NUM_BYTES)
#define EV_SLP_NUM_BYTES 			(2)
#define IN_SLP_ADDR					(EV_SLP_ADDR+EV_SLP_NUM_BYTES)
#define IN_SLP_NUM_BYTES			(2)
#define ISL9122_ADDR				(IN_SLP_ADDR+IN_SLP_NUM_BYTES)
#define ISL9122_NUM_BYTES			(1)
#define POL_MET_ADDR				(ISL9122_ADDR+ISL9122_NUM_BYTES)
#define POL_MET_NUM_BYTES			(1)
#define ENCRYPTED_KEY_ADDR			(POL_MET_ADDR+POL_MET_NUM_BYTES)
#define ENCRYPTED_KEY_NUM_BYTES		(16)
#define TX_DBM_ADDR					(ENCRYPTED_KEY_ADDR+ENCRYPTED_KEY_NUM_BYTES)
#define TX_DBM_NUM_BYTES			(1)
#define NAME_ADDR					(TX_DBM_ADDR+TX_DBM_NUM_BYTES)
#define NAME_NUM_BYTES				(10)
#define VBULK_THRESH_ADDR			(NAME_ADDR+NAME_NUM_BYTES)
#define VBULK_THRESH_BYTES			(1)
#define NEG_EVT_CTR_ADDR			(VBULK_THRESH_ADDR+VBULK_THRESH_BYTES)
#define NEG_EVT_CTR_BYTES			(4)
#define POS_EVT_CTR_ADDR			(NEG_EVT_CTR_ADDR+NEG_EVT_CTR_BYTES)
#define POS_EVT_CTR_BYTES			(4)


/**
 * @brief Structure representing the data format which is stored inside the FRAM
 * 
 */
typedef struct
{
    uint32_t event_counter;                                 // Lifetime counter of number of events processed
    uint32_t serial_number;                                 // Manufacturing serial number
    uint8_t  type;                                          // Packet type  
    uint8_t  packet_interval;                               // Advertising interval in milliseconds
    uint16_t event_max_packets;                             // Maximum number of packet repeats per eveents
    uint16_t sleep_between_events;                          // Minimum sleep time before next event in milliseconds
    uint16_t sleep_after_wake;                              // Sleep time before polarity detection in milliseconds
	uint8_t  u8_voltsISL9122;                               // Voltage of ISL9122
	uint8_t  u8_POLmethod;                                  // Polling method (GPIO or comparator)
	uint8_t  encrypted_key[ENCRYPTED_KEY_NUM_BYTES];        // Encrypted key - AES -128
	uint8_t  tx_dbm_10;                                     // TX power in 0.1dBm
	uint8_t  cName[NAME_NUM_BYTES];                         // Name for the alert sensor types
    uint8_t  vbulk_thresh;                                  // VBULK Threshold
    uint32_t negative_events_counter;                       // Counter indicating number of negative events
    uint32_t positive_events_counter;                       // Counter indicating number of positive events
} fram_data_t;

/**
 * @brief Enumeration for the type of feilds in FRAM
 * 
 */
enum FRAM_FIELDS 
{
    EV_CTR = 0,          // Event Counter
    SER_NUM,             // Serial Number
    TYPE,                // Type
    EV_INT,              // Event Interval    
    EV_MAX,              // Event Max packets
    EV_SLP,              // Event Sleep Time
    IN_SLP,              // Sleep After Wake
    ISL9122,             // Voltage of ISL9122
    POL_MET,             // Polling Method
    ENCRYPTED_KEY,       // Encrypted Key
    TX_DBM,              // TX Power dbm
    NAME,                //  Name
    VBULK_THRESH,        // VBULK Thresh
    NEG_EVT_CTR,         // negative event counter
    POS_EVT_CTR,         // positive event counter
    MAX_FRAM_FIELDS      // Maximum FRAM fields
};

/**
 * @brief Method to read a particular field in FRAM
 * 
 * @param field Field number in FRAM to read the data
 * @param read_buffer Buffer containing the read data
 * @return int Error code
 */
int app_fram_read_field(uint8_t field, uint8_t *read_buffer);

/**
 * @brief Method to write data to a certain feild in FRAM
 * 
 * @param field Feild in FRAM to write the data
 * @param data_to_write Data to write in FRAM
 * @return int error code
 */
int app_fram_write_field(uint8_t field,  uint8_t *data_to_write);

/**
 * @brief FRAM service funtion used to increment the event counter independently 
 * 
 * @param counter New counter value - after increment 
 * @return int error code
 */
int app_fram_service(uint32_t *counter);

/**
 * @brief Method to the data from FRAM via I2C and store it in fram_data_t buffer
 * 
 * @param read_buffer Buffer containing the read data from FRAM
 * @return int error code
 */
int app_fram_read_data(fram_data_t *read_buffer);

/**
 * @brief Methode to write the fram_data_t buffer in the FRAM via I2C
 * 
 * @param buffer_to_write Buffer containing the data to write
 * @return int error code
 */
int app_fram_write_data( fram_data_t *buffer_to_write);

/**
 * @brief Read event counter value from FRAM
 * 
 * @param fram_buffer fram_data_t buffer to store the new counter value
 * @return int error code
 */
int app_fram_read_counter(fram_data_t *fram_buffer);

/**
 * @brief Write event counter value in FRAM
 * 
 * @param new_fram_buffer Buffer containing the new counter value to be stored in FRAM 
 * @return int error code
 */
int app_fram_write_counter( fram_data_t *new_fram_buffer);

/**
 * @brief Write negative_events_counter value in FRAM
 * 
 * @param new_fram_buffer Buffer containing the new negative_events_counter value to be stored in FRAM 
 * @return int error code
 */
int app_fram_write_counter_neg( fram_data_t *new_fram_buffer);

/**
 * @brief Write positive_events_counter value in FRAM
 * 
 * @param new_fram_buffer Buffer containing the new positive_events_counter value to be stored in FRAM 
 * @return int error code
 */
int app_fram_write_counter_pos( fram_data_t *new_fram_buffer);

/**
 * @brief Write polarity counters in FRAM
 * 
 * @param new_fram_buffer Buffer containing New polarity counter values which will be stored in FRAM
 */
void fram_update_pol_counters(fram_data_t new_fram_buffer);

#endif // __FRAM__

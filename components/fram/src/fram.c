#include "fram.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/types.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/i2c.h>

#include "device_config.h"
#include "app_types.h"
#include "config_commands.h"

// FRAM Defines
#define FRAM_I2C_ADDR 0x50

#define FRAM_WRITE_ADDR_BYTES		2
#define FRAM_I2C_MSG_BYTES			2
#define FRAM_I2C_WRITE_NO_OF_MSGS	2

LOG_MODULE_DECLARE(wepower);

/**
 * @brief I2C FRAM Write bytes
 * 
 * @param i2c_dev   I2C Device to send data
 * @param addr      Address to write
 * @param data      Data to write
 * @param num_bytes Number of bytes to write
 * @param device_addr  Device Address to write
 * @return int      Error Code
 */
static int i2c_fram_write_bytes(const struct device *i2c_dev, uint16_t addr, uint8_t *data, uint32_t num_bytes, uint8_t device_addr)
{
	uint8_t wr_addr[FRAM_WRITE_ADDR_BYTES];
	struct i2c_msg msgs[FRAM_I2C_MSG_BYTES];

	/* FRAM address */
	wr_addr[0] = (addr >> 8) & 0xFF;
	wr_addr[1] = addr & 0xFF;
    
	/* Setup I2C messages */

	/* Send the address to write to */
	msgs[0].buf = wr_addr;
	msgs[0].len = FRAM_WRITE_ADDR_BYTES;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Data to be written, and STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], FRAM_I2C_WRITE_NO_OF_MSGS, device_addr);
}

/**
 * @brief I2C FRAM Read bytes
 * 
 * @param i2c_dev   I2C Device to read from
 * @param addr      Address to read from
 * @param data      Data buffer to store read data
 * @param num_bytes Number of bytes to read
 * @param device_addr  Device Address to read from
 * @return int 
 */
static int i2c_fram_read_bytes(const struct device *i2c_dev, uint16_t addr, uint8_t *data, uint32_t num_bytes, uint8_t device_addr)
{
	uint8_t wr_addr[FRAM_WRITE_ADDR_BYTES];
	struct i2c_msg msgs[FRAM_I2C_MSG_BYTES];

	/* FRAM address to read from */
	wr_addr[0] = (addr >> 8) & 0xFF;
	wr_addr[1] = addr & 0xFF;

	/* Setup I2C messages */

	/* Send the address to read from */
	msgs[0].buf = wr_addr;
	msgs[0].len = FRAM_WRITE_ADDR_BYTES;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Read from device. STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], FRAM_I2C_WRITE_NO_OF_MSGS, device_addr);
}

/**
 * @brief Get the field addr and length based on the field type
 * 
 * @param field_type The type of field to get address and length of
 * @param field_addr Buffer to store the address of field in FRAM
 * @param field_length Buffer to store the length/size of the field in FRAM
 */
static void get_field_addr_and_length_based_on_type(uint8_t field_type, uint16_t* field_addr, uint32_t* field_length)
{
	switch(field_type)
	{
		case EV_CTR:
			*field_addr = FRAM_COUNTER_ADDR;
			*field_length = FRAM_COUNTER_NUM_BYTES;
			break;
		case SER_NUM:
			*field_addr = SER_NUM_ADDR;
			*field_length = SER_NUM_BYTES;
			break;
		case TYPE:
			*field_addr = TYPE_ADDR;
			*field_length = TYPE_NUM_BYTES;
			break;
		case EV_INT:
			*field_addr = EV_INT_ADDR;
			*field_length = EV_INT_NUM_BYTES;
			break;
		case EV_MAX:
			*field_addr = EV_MAX_ADDR;
			*field_length = EV_MAX_NUM_BYTES;
			break;
		case EV_SLP:
			*field_addr = EV_SLP_ADDR;
			*field_length = EV_SLP_NUM_BYTES;
			break;
		case IN_SLP:
			*field_addr = IN_SLP_ADDR;
			*field_length = IN_SLP_NUM_BYTES;
			break;
		case ISL9122:
			*field_addr = ISL9122_ADDR;
			*field_length = ISL9122_NUM_BYTES;
			break;
		case POL_MET:
			*field_addr = POL_MET_ADDR;
			*field_length = POL_MET_NUM_BYTES;
			break;
		case ENCRYPTED_KEY:
			*field_addr = ENCRYPTED_KEY_ADDR;
			*field_length = ENCRYPTED_KEY_NUM_BYTES;
			break;
		case TX_DBM:
			*field_addr = TX_DBM_ADDR;
			*field_length = TX_DBM_NUM_BYTES;
			break;
		case NAME:
			*field_addr = NAME_ADDR;
			*field_length = NAME_NUM_BYTES;
			break;
		case NEG_EVT_CTR:
			*field_addr = NEG_EVT_CTR_ADDR;
			*field_length = NEG_EVT_CTR_BYTES;
			break;
		case POS_EVT_CTR:
			*field_addr = POS_EVT_CTR_ADDR;
			*field_length = POS_EVT_CTR_BYTES;
			break;
		case VBULK_THRESH:
			*field_addr = VBULK_THRESH_ADDR;
			*field_length = VBULK_THRESH_BYTES;
			break;
		default:
			*field_addr = 0;
			*field_length = 0;
			break;
	}
}

/**
 * @brief Method to read a particular field in FRAM
 * 
 * @param field Field number in FRAM to read the data
 * @param read_buffer Buffer containing the read data
 * @return int Error code
 */
int app_fram_read_field(uint8_t field, uint8_t *read_buffer)
{
	int ret;
	uint16_t addr = 0;
	uint32_t length = 0;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Reading from FRAM failed - I2C device is not ready");
		return FRAM_ERROR;
	}

	get_field_addr_and_length_based_on_type(field, &addr, &length);

	ret = i2c_fram_read_bytes(i2c_dev, addr, read_buffer, length, FRAM_I2C_ADDR);
	if (ret) 
	{
        LOG_ERR("Error reading from FRAM! error code (%d)\n", ret);
		return FRAM_ERROR;
	} 
	else
	{
        LOG_INF(">>Read the data successfully\n");
	}

	return FRAM_SUCCESS;
}

/**
 * @brief Method to write data to a certain feild in FRAM
 * 
 * @param field Feild in FRAM to write the data
 * @param data_to_write Data to write in FRAM
 * @return int error code
 */
int app_fram_write_field(uint8_t field,  uint8_t *data_to_write)
{
	int ret;
	uint16_t addr = 0;
	uint32_t num_bytes = 0;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Writing to FRAM failed - I2C device is not ready");
		return FRAM_ERROR;
	}

	get_field_addr_and_length_based_on_type(field, &addr, &num_bytes);

	ret = i2c_fram_write_bytes(i2c_dev, addr, data_to_write, num_bytes, FRAM_I2C_ADDR);
	if (ret)
	{
        LOG_ERR("Error writing from FRAM! error code (%d)", ret);
	}
	else 
	{
		LOG_INF(">>Write the data successfully");
	}
	return FRAM_SUCCESS;
}

/**
 * @brief Method to the data from FRAM via I2C and store it in fram_data_t buffer
 * 
 * @param read_buffer Buffer containing the read data from FRAM
 * @return int error code
 */
int app_fram_read_data(fram_data_t *read_buffer)
{
	int ret;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Reading FRAM data failed - I2C device not ready");
		return FRAM_ERROR;
	}

	ret = i2c_fram_read_bytes(i2c_dev, FRAM_COUNTER_ADDR, (uint8_t*)read_buffer, sizeof(fram_data_t), FRAM_I2C_ADDR);
	if (ret) 
	{
        LOG_ERR("Error reading from FRAM! error code (%d)", ret);
		return FRAM_ERROR;
	} 
	return FRAM_SUCCESS;
}

/**
 * @brief Methode to write the fram_data_t buffer in the FRAM via I2C
 * 
 * @param buffer_to_write Buffer containing the data to write
 * @return int error code
 */
int app_fram_write_data( fram_data_t *buffer_to_write)
{
	int ret;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev))
	{
		LOG_ERR("FRAM write data failed - I2C device not ready");
		return FRAM_ERROR;
	}

	ret = i2c_fram_write_bytes(i2c_dev, FRAM_COUNTER_ADDR, (uint8_t*)buffer_to_write, sizeof(fram_data_t), FRAM_I2C_ADDR);
	if (ret) 
	{
		LOG_ERR("Error writing to FRAM! error code (%d)", ret);
		return FRAM_ERROR;
	} 
	else
	{
        LOG_INF(">> ------- Writing FRAM Data -------");
        LOG_INF(">>[FRAM INFO]->Event Counter: 0x%08X", buffer_to_write->event_counter);
		LOG_INF(">>[FRAM INFO]->Serial Number: 0x%08X", buffer_to_write->serial_number);
		LOG_INF(">>[FRAM INFO]->Device Type: %d", buffer_to_write->type);
		LOG_INF(">>[FRAM INFO]->Frame Interval: %d ms", buffer_to_write->packet_interval);
		LOG_INF(">>[FRAM INFO]->Frame Maximum Number: %d", buffer_to_write->event_max_packets);
		LOG_INF(">>[FRAM INFO]->Minimum Sleeping Interval: %d", buffer_to_write->sleep_between_events);
		LOG_INF(">>[FRAM INFO]->Sleep time After Wake up: %d", buffer_to_write->sleep_after_wake);
		LOG_INF(">>[FRAM INFO]->Voltage of ISL9122: %d", buffer_to_write->u8_voltsISL9122);
		LOG_INF(">>[FRAM INFO]->POL Method: %d", buffer_to_write->u8_POLmethod);
		LOG_INF(">>[FRAM INFO]->Encrypted Key: ");
		LOG_INF(">>[FRAM INFO]->TX dBM 10: %d", buffer_to_write->tx_dbm_10);
		LOG_INF(">>[FRAM INFO]->cName: %s", buffer_to_write->cName); 
		LOG_INF(">>[FRAM INFO]->negative_events_counter: %d", fram_data.negative_events_counter);
        LOG_INF(">>[FRAM INFO]->positive_events_counter: %d", fram_data.positive_events_counter);
		return FRAM_SUCCESS;
	}
}

/**
 * @brief Read event counter value from FRAM
 * 
 * @param fram_buffer fram_data_t buffer to store the new counter value
 * @return int error code
 */
int app_fram_read_counter(fram_data_t *fram_buffer)
{
	int ret;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("Reading FRAM counter value failed - I2C device not ready");
		return FRAM_ERROR;
	}

	ret = i2c_fram_read_bytes(i2c_dev, FRAM_COUNTER_ADDR, (uint8_t*)fram_buffer, FRAM_COUNTER_NUM_BYTES, FRAM_I2C_ADDR);
	if (ret) 
	{
		LOG_ERR("Error reading FRAM counter value error code %d",ret);
		return FRAM_ERROR;
	} 
	else 
	{
        LOG_PRINTK(">>[FRAM INFO]->Frame Counter: 0x%08X", fram_buffer->event_counter);
	}

	return FRAM_SUCCESS;
}



/**
 * @brief Write event counter value in FRAM
 * 
 * @param new_fram_buffer Buffer containing the new counter value to be stored in FRAM 
 * @return int error code
 */
int app_fram_write_counter( fram_data_t *new_fram_buffer)
{
	int ret;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev))
	{
		LOG_ERR("Writing FRAM counter value failed - I2C device not ready");
		return FRAM_ERROR;
	}

	ret = i2c_fram_write_bytes(i2c_dev, FRAM_COUNTER_ADDR, (uint8_t*)new_fram_buffer, FRAM_COUNTER_NUM_BYTES, FRAM_I2C_ADDR);
	if (ret) 
	{
		LOG_ERR("Writing FRAM counter failed, error code %d", ret);
		return FRAM_ERROR;
	}
	else
	{
        LOG_PRINTK(">>[FRAM INFO]->Frame Counter: 0x%08X", new_fram_buffer->event_counter);
	}

	return FRAM_SUCCESS;
}

/**
 * @brief Write positive_events_counter value in FRAM
 * 
 * @param new_fram_buffer Buffer containing the new positive_events_counter value to be stored in FRAM 
 * @return int error code
 */
int app_fram_write_counter_pos( fram_data_t *new_fram_buffer)
{
	uint32_t positive_events_counter_to_write = new_fram_buffer->positive_events_counter;  

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev))
	{
		LOG_ERR("Writing FRAM Pos counter value failed - I2C device not ready");
		return FRAM_ERROR;
	}

	if (i2c_fram_write_bytes(i2c_dev, POS_EVT_CTR_ADDR, (uint8_t*)(&(positive_events_counter_to_write)), POS_EVT_CTR_BYTES, FRAM_I2C_ADDR)) 
	{
		return FRAM_ERROR;
	}
	else
	{
        LOG_PRINTK(">>[FRAM INFO]->Frame positive_events_counter: 0x%08X", new_fram_buffer->positive_events_counter);
	}

	return FRAM_SUCCESS;
}

/**
 * @brief Write negative_events_counter value in FRAM
 * 
 * @param new_fram_buffer Buffer containing the new negative_events_counter value to be stored in FRAM 
 * @return int error code
 */
int app_fram_write_counter_neg( fram_data_t *new_fram_buffer)
{
	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	uint32_t negative_events_counter_to_write = new_fram_buffer->negative_events_counter; 

	if (!device_is_ready(i2c_dev))
	{
		LOG_ERR("Writing FRAM Neg counter value failed - I2C device not ready");
		return FRAM_ERROR;
	}

	if (i2c_fram_write_bytes(i2c_dev, NEG_EVT_CTR_ADDR, (uint8_t*)(&(negative_events_counter_to_write)), NEG_EVT_CTR_BYTES, FRAM_I2C_ADDR)) 
	{
		return FRAM_ERROR;
	}
	else
	{
        LOG_PRINTK(">>[FRAM INFO]->Frame negative_events_counter: 0x%08X", new_fram_buffer->negative_events_counter);
	}

	return FRAM_SUCCESS;
}

/**
 * @brief FRAM service funtion used to increment the event counter independently 
 * 
 * @param counter New counter value - after increment 
 * @return int error code
 */
int app_fram_service(uint32_t *counter)
{
	int ret;
    u32_u8_t fram_data;
    fram_data.u32 = 0;

	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

	if (!device_is_ready(i2c_dev)) 
	{
		LOG_ERR("FRAM service error - I2C device not ready");
		return FRAM_ERROR;
	}

    // Read the 4 data bytes at this address
	ret = i2c_fram_read_bytes(i2c_dev, FRAM_COUNTER_ADDR, &fram_data.u8[0], FRAM_COUNTER_NUM_BYTES, FRAM_I2C_ADDR);
	if (ret) 
	{
		LOG_ERR("Error reading FRAM counter value in FRAM service, error code %d", ret);
		return FRAM_ERROR;
	} 
	else 
	{
        LOG_INF("Read %u (%08X)from address %u.", fram_data.u32, fram_data.u32, FRAM_COUNTER_ADDR);
	}

    // Increment count
    fram_data.u32++;

	ret = i2c_fram_write_bytes(i2c_dev, FRAM_COUNTER_ADDR, &fram_data.u8[0], FRAM_COUNTER_NUM_BYTES, FRAM_I2C_ADDR);
	if (ret) 
	{
		LOG_ERR("Error writing FRAM counter value in FRAM service, error code %d", ret);
		return FRAM_ERROR;
	}
	else
	{
        LOG_INF("Wrote %u (%08X) to address %u.", fram_data.u32, fram_data.u32, FRAM_COUNTER_ADDR);
	}

    *counter = fram_data.u32;

    return FRAM_SUCCESS;
}

/**
 * @brief Write polarity counters in FRAM
 * 
 * @param new_fram_buffer Buffer containing New polarity counter values which will be stored in FRAM
 */
void fram_update_pol_counters(fram_data_t *new_fram_buffer)
{
	app_fram_write_counter_neg(new_fram_buffer);
	app_fram_write_counter_pos(new_fram_buffer);
}
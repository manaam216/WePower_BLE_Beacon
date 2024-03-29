#include "app_bt.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <hal/nrf_gpio.h>

#include "config_commands.h"
#include "device_config.h"
#include "error_output.h"

#include "app_manuf_data.h"
#include "app_burn_energy.h"
#include "app_gpio.h"

#define BT_UUID_BYTE1   0x50
#define BT_UUID_BYTE2   0x57

LOG_MODULE_DECLARE(wepower);

struct bt_le_adv_param adv_param =
		BT_LE_ADV_PARAM_INIT(
				     BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_USE_IDENTITY | BT_LE_ADV_OPT_USE_NAME,
				     BLE_ADV_INTERVAL_MIN,
				     BLE_ADV_INTERVAL_MAX,
				     NULL);

static struct bt_data ad[] = 
{
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, manufacture_data, PAYLOAD_FRAME_LENGTH),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_BYTE1, BT_UUID_BYTE2)
};

static struct bt_le_ext_adv *ext_adv;

struct k_work start_advertising_work_item;

/**
 * @brief Callback which is hit after every advertising event
 * 
 * @param instance Insatance for the bluetooth advertising 
 * @param info Information for the advertising event
 */
static void adv_sent_cb(struct bt_le_ext_adv *instance, struct bt_le_ext_adv_sent_info *info)
{	
	clear_CN1_6();
    LOG_INF("Advertiser[%d] %p sent %d\n", bt_le_ext_adv_get_index(ext_adv), (void*)ext_adv, info->num_sent);
}

static const struct bt_le_ext_adv_cb adv_callback = {.sent = adv_sent_cb,};

/**
 * @brief Create a advertising object
 * 
 * @return int error code
 */
static int create_advertising(void)
{
	return bt_le_ext_adv_create(&adv_param, &adv_callback, &ext_adv);
}

/**
 * @brief Function used to start the advertising on Bluetooth
 * 
 * @param work Work Item from work queue
 */
void start_advertising_handler(struct k_work *work)
{
    set_CN1_6();
    clear_CN1_6();

    LOG_INF("Manufacturer Data: ");
    for (uint8_t i = 0; i < PAYLOAD_FRAME_LENGTH; i++){
        LOG_RAW("%02X ", manufacture_data[i]);
    }
    LOG_RAW(" \n");

    
    LOG_INF("Start_Advertising->Setting Data");

	if (bt_le_ext_adv_set_data(ext_adv, ad, ARRAY_SIZE(ad), NULL, 0)) 
    {
        LOG_ERR("Failed to set advertising data");
	}

    LOG_INF("Start_Advertising->BLE ADV Start");
	if (bt_le_ext_adv_start(ext_adv, BT_LE_EXT_ADV_START_PARAM(BLE_ADV_TIMEOUT, BLE_ADV_EVENTS))) 
    {
		LOG_ERR("Failed to start advertising set \n");
	}
}

/**
 * @brief BT ready function. Should be called after bluetooth is enabled
 *        successfully. 
 * 
 * @return int error code
 */
int bt_ready(void)
{
	int err = -1;

	LOG_INF("Bluetooth initialized");

	err = create_advertising();
	if (err)
    {
		LOG_ERR("Advertising failed to create (err %d)", err);	
	}
    
    return err;
}

/**
 * @brief Initialize Bluetooth for WePower Board
 * 
 * @return uint8_t error code, 0 if successful
 */
uint8_t initialize_bluetooth()
{
    uint8_t err = 0xFF;
    // Init and run the BLE
    err = bt_enable(NULL);
    if (err) 
    {
        LOG_ERR("Bluetooth init failed (err %d)\n", err);
        indicate_error(ERROR_TYPE_BT_ENABLE_FAIL);
        burn_the_energy();
    }

    //Create Advertiser Set Create
    err = bt_ready();
    if (err)
    {
        LOG_ERR("Advertising failed to create (err %d)\n", err);
        indicate_error(ERROR_TYPE_BT_READY_FAIL);
        burn_the_energy();
    }

    return err;
}
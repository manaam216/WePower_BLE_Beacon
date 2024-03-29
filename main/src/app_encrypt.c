#include "app_encrypt.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/logging/log.h>
#include <hal/nrf_gpio.h>

#include "encrypt.h"
#include "device_config.h"

#include "app_manuf_data.h"
#include "app_gpio.h"

LOG_MODULE_DECLARE(wepower);


/**
 * @brief Encrypt the data and store in the buffer
 * 
 * @param clear_text_buf     Buffer containing un-encrypted text
 * @param encrypted_text_buf Buffer containing encrypted text
 * @param len                Length of the text to encrypt 
 */
void encrypt_data(uint8_t* clear_text_buf, uint8_t* encrypted_text_buf, uint8_t len )
{
    set_CN1_7();
#define ENCRYPT 0
#ifdef ENCRYPT
    // Not we want to encrypt the data
    if(app_encrypt_payload(clear_text_buf, len, encrypted_text_buf, len) == ENCRYPTION_ERROR)
    {
        memcpy(encrypted_text_buf, clear_text_buf, PAYLOAD_DATA_SIZE_BYTES);
        manufacture_data[PAYLOAD_STATUS_BYTE_INDEX] = PAYLOAD_ENCRYPTION_STATUS_CLEAR;
    }
    else
    {
        manufacture_data[PAYLOAD_STATUS_BYTE_INDEX] = PAYLOAD_ENCRYPTION_STATUS_ENC;
    }
#else
    memcpy(cipher_text, encrypted_text_buf, PAYLOAD_DATA_SIZE_BYTES);
    manf_data[PAYLOAD_STATUS_BYTE_INDEX] = PAYLOAD_ENCRYPTION_STATUS_CLEAR;
#endif
    LOG_INF("Payload - Cleartext: ");
    for(int i = 0; i < len; i++)
    {
        LOG_RAW("%02X ", clear_text_buf[i]);
    }
    LOG_RAW("\n");

    LOG_INF("Payload - Encrypted: ");
    for(int i = 0; i < len; i++)
    {
        LOG_RAW("%02X ", encrypted_text_buf[i]);
    }
    LOG_RAW("\n");
    clear_CN1_7();
}
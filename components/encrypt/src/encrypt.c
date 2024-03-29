#include "encrypt.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/crypto/crypto.h>

#include "device_config.h"
#include "app_types.h"

#define CRYPTO_DRV_NAME CONFIG_CRYPTO_MBEDTLS_SHIM_DRV_NAME
#define CRYPTO_DEV_COMPAT nordic_nrf_ecb

LOG_MODULE_DECLARE(wepower);

/**
 * @brief Buffer used to store encrypted key. Currently it is 
 * having default values. These wvalue will be overwritten after very boot
 * 
 */
uint8_t ecb_key[ENCRYPTED_KEY_SIZE] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

/**
 * @brief Routine used to query hardware caps and check for compatibility
 * 
 * @param dev 		Crypto device driver
 * @return int8_t 	error code, 0 if success 
 */
static int8_t validate_hw_compatibility(const struct device *dev)
{
	uint32_t flags = 0U;

	flags = crypto_query_hwcaps(dev);

	if ((flags & CAP_RAW_KEY) == 0U) 
	{
        LOG_ERR("Please provision the key separately "
				"as the module doesnt support a raw key");
		return ENCRYPTION_ERROR;
	}

	if ((flags & CAP_SYNC_OPS) == 0U) 
	{
		LOG_ERR("The app assumes sync semantics. "
		  		"Please rewrite the app accordingly before proceeding");
		return ENCRYPTION_ERROR;
	}

	if ((flags & CAP_SEPARATE_IO_BUFS) == 0U) 
	{
		LOG_ERR("The app assumes distinct IO buffers. "
				"Please rewrite the app accordingly before proceeding");
		return ENCRYPTION_ERROR;
	}
	return ENCRYPTION_SUCCESS;
}

/**
 * @brief Encrypt the input text into encrypted text
 * 
 * @param cleartext     Un-encrypted text
 * @param cleartext_len Un-encrypted text length 
 * @param encrypted     Encrypted text
 * @param encrypted_len Encrypted text length
 * @return int 			error code
 **/
int app_encrypt_payload(uint8_t *cleartext, uint8_t cleartext_len, uint8_t *encrypted, uint8_t encrypted_len)
{
    const struct device *dev_crypto = device_get_binding(CRYPTO_DRV_NAME);

	if (!dev_crypto) 
	{
		LOG_ERR("%s crypto device not found", CRYPTO_DRV_NAME);
		return ENCRYPTION_ERROR;
	}

    if (validate_hw_compatibility(dev_crypto)) 
	{
		LOG_ERR("Hardware compatibility check failed");
		return ENCRYPTION_ERROR;
	}

    LOG_INF("Crypto initialization successful");

	struct cipher_ctx init_context =
	{
		.keylen = sizeof(ecb_key),
		.key.bit_stream = ecb_key,
		.flags = CAP_RAW_KEY | CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS,
	};
	struct cipher_pkt encrypt_pkt = 
	{
		.in_buf = cleartext,
		.in_len = cleartext_len,
		.out_buf_max = encrypted_len,
		.out_buf = encrypted,
	};

    if (cipher_begin_session(dev_crypto, &init_context, CRYPTO_CIPHER_ALGO_AES,
				 CRYPTO_CIPHER_MODE_ECB,
				 CRYPTO_CIPHER_OP_ENCRYPT) != 0)
	{
		return ENCRYPTION_ERROR;
	}

	if (cipher_block_op(&init_context, &encrypt_pkt)) 
	{
		LOG_ERR("ERROR: ECB mode ENCRYPT - Failed\n");
		goto out;
	}

    LOG_INF("Output length (encryption): %d\n", encrypt_pkt.out_len);

	if (memcmp(encrypt_pkt.out_buf, encrypted, encrypted_len)) 
	{
		LOG_ERR("ERROR: ECB mode ENCRYPT - Mismatch between expected and "
			    "returned cipher text\n");
		goto out;
	}

	LOG_INF("ECB mode ENCRYPT - Match\n");
    cipher_free_session(dev_crypto, &init_context);
    return ENCRYPTION_SUCCESS;

out:
	cipher_free_session(dev_crypto, &init_context);
    return ENCRYPTION_ERROR;
}
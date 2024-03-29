#ifndef __APP_ENCRYPT__
#define __APP_ENCRYPT__

#include <zephyr/kernel.h>


/**
 * @brief Encrypt the data and store in the buffer
 * 
 * @param clear_text_buf     Buffer containing un-encrypted text
 * @param encrypted_text_buf Buffer containing encrypted text
 * @param len                Length of the text to encrypt 
 */
void encrypt_data(uint8_t* clear_text_buf, uint8_t* encrypted_text_buf, uint8_t len );

#endif // __APP_ENCRYPT__

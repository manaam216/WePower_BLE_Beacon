#ifndef __ENCRYPT__
#define __ENCRYPT__

#include <stdint.h>

#define ENCRYPTED_KEY_SIZE 16

extern uint8_t ecb_key[ENCRYPTED_KEY_SIZE];

#define ENCRYPTION_ERROR -1
#define ENCRYPTION_SUCCESS 0

/**
 * @brief Encrypt the input text into encrypted text
 * 
 * @param cleartext     Un-encrypted text
 * @param cleartext_len Un-encrypted text length 
 * @param encrypted     Encrypted text
 * @param encrypted_len Encrypted text length
 * @return int  error code 
 */
int app_encrypt_payload(uint8_t *cleartext, uint8_t cleartext_len, uint8_t *encrypted, uint8_t encrypted_len);


#endif // __ENCRYPT__

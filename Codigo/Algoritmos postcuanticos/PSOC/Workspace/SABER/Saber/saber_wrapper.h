#ifndef SABER_WRAPPER_H
#define SABER_WRAPPER_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Available security levels for SABER
typedef enum {
    SABER_SECURITY_LIGHT = 2,  // L=2 for LightSaber
    SABER_SECURITY_SABER = 3,  // L=3 for Saber
    SABER_SECURITY_FIRE = 4    // L=4 for FireSaber
} saber_security_level_t;

// All functions return 0 on success, nonzero on error
// pk Pointer to output public key (allocated by caller)
// sk Pointer to output secret key (allocated by caller)
// ct Pointer to output ciphertext (allocated by caller)
// ss Pointer to output shared secret (allocated by caller)
int saber_init(saber_security_level_t security_level);
size_t saber_get_public_key_size(void);
size_t saber_get_secret_key_size(void);
size_t saber_get_ciphertext_size(void);
size_t saber_get_shared_secret_size(void);
int saber_keypair(unsigned char *pk, unsigned char *sk);
int saber_encapsulate(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int saber_decapsulate(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

#ifdef __cplusplus
}
#endif

#endif /* SABER_WRAPPER_H */
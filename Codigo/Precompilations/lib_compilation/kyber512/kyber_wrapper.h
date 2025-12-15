#ifndef KYBER_WRAPPER_H
#define KYBER_WRAPPER_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//Available security levels for Kyber
typedef enum {
    KYBER_SECURITY_512 = 2,  // K=2 for Kyber-512
    KYBER_SECURITY_768 = 3,  // K=3 for Kyber-768
    KYBER_SECURITY_1024 = 4  // K=4 for Kyber-1024
} kyber_security_level_t;

//In all function 0 on success, nonzero on error
//pk Pointer to output public key (allocated by caller)
//sk Pointer to output secret key (allocated by caller)
//ct Pointer to output ciphertext (allocated by caller)
//ss Pointer to output shared secret (allocated by caller) 
int kyber_init(kyber_security_level_t security_level);
size_t kyber_get_public_key_size(void);
size_t kyber_get_secret_key_size(void);
size_t kyber_get_ciphertext_size(void);
size_t kyber_get_shared_secret_size(void);
int kyber_keypair(unsigned char *pk, unsigned char *sk);
int kyber_encapsulate(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int kyber_decapsulate(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);


#ifdef __cplusplus
}
#endif

#endif /* KYBER_WRAPPER_H */
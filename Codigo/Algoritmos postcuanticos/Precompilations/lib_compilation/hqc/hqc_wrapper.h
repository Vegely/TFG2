#ifndef HQC_WRAPPER_H
#define HQC_WRAPPER_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// HQC security levels (HQC-256 is the only one we support)
typedef enum {
    HQC_SECURITY_128 = 128,  // HQC-128
    HQC_SECURITY_192 = 192,  // HQC-192
    HQC_SECURITY_256 = 256   // HQC-256
} hqc_security_level_t;

// All functions return 0 on success, nonzero on error
// pk: Pointer to output public key (allocated by caller)
// sk: Pointer to output secret key (allocated by caller)
// ct: Pointer to output ciphertext (allocated by caller)
// ss: Pointer to output shared secret (allocated by caller)


int hqc_init(hqc_security_level_t security_level);
size_t hqc_get_public_key_size(void);
size_t hqc_get_secret_key_size(void);
size_t hqc_get_ciphertext_size(void);
size_t hqc_get_shared_secret_size(void);
int hqc_keypair(unsigned char *pk, unsigned char *sk);
int hqc_encapsulate(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int hqc_decapsulate(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

#ifdef __cplusplus
}
#endif

#endif /* HQC_WRAPPER_H */
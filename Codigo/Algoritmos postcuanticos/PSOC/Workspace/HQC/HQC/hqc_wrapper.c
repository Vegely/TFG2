#include "hqc_wrapper.h"
#include "api.h"

// Current security level
static hqc_security_level_t current_security_level = HQC_SECURITY_256;

// Static flag to indicate if the library has been initialized
static int is_initialized = 0;

int hqc_init(hqc_security_level_t security_level) {
    // Validate security level
    if (security_level != HQC_SECURITY_128 && 
        security_level != HQC_SECURITY_192 && 
        security_level != HQC_SECURITY_256) {
        return -1;
    }
    
    // Store security level (for runtime checking)
    current_security_level = security_level;
    is_initialized = 1;
    
    return 0;
}

size_t hqc_get_public_key_size(void) {
    return PQCLEAN_HQC256_CLEAN_CRYPTO_PUBLICKEYBYTES;
}

size_t hqc_get_secret_key_size(void) {
    return PQCLEAN_HQC256_CLEAN_CRYPTO_SECRETKEYBYTES;
}

size_t hqc_get_ciphertext_size(void) {
    return PQCLEAN_HQC256_CLEAN_CRYPTO_CIPHERTEXTBYTES;
}

size_t hqc_get_shared_secret_size(void) {
    return PQCLEAN_HQC256_CLEAN_CRYPTO_BYTES;
}

int hqc_keypair(unsigned char *pk, unsigned char *sk) {
    if (!is_initialized) return -1;
    if (pk == NULL || sk == NULL) return -2;
    
    int result = PQCLEAN_HQC256_CLEAN_crypto_kem_keypair(pk, sk);
    return result;
}

int hqc_encapsulate(unsigned char *ct, unsigned char *ss, const unsigned char *pk) {
    if (!is_initialized) return -1;
    if (ct == NULL || ss == NULL || pk == NULL) return -2;
    
    int result = PQCLEAN_HQC256_CLEAN_crypto_kem_enc(ct, ss, pk);
    return result;
}

int hqc_decapsulate(unsigned char *ss, const unsigned char *ct, const unsigned char *sk) {
    if (!is_initialized) return -1;
    if (ss == NULL || ct == NULL || sk == NULL) return -2;
    
    int result = PQCLEAN_HQC256_CLEAN_crypto_kem_dec(ss, ct, sk);
    if (result != 0) return -3; // Decapsulation failed
    
    return 0;
}
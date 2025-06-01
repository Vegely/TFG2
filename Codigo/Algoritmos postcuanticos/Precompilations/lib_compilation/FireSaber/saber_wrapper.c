#include "saber_wrapper.h"
#include "SABER_params.h"
#include "api.h"

// Current security level
static saber_security_level_t current_security_level = SABER_SECURITY_FIRE;

// Static flag to indicate if the library has been initialized
static int is_initialized = 0;

int saber_init(saber_security_level_t security_level) {
    // Validate security level
    if (security_level != SABER_SECURITY_LIGHT && 
        security_level != SABER_SECURITY_SABER && 
        security_level != SABER_SECURITY_FIRE) {
        return -1;
    }
    
    // Check if the compiled version matches the requested security level
    #if SABER_L == 2
        if (security_level != SABER_SECURITY_LIGHT) {
            return -2; // Compiled for LightSaber but different level requested
        }
    #elif SABER_L == 3
        if (security_level != SABER_SECURITY_SABER) {
            return -2; // Compiled for Saber but different level requested
        }
    #elif SABER_L == 4
        if (security_level != SABER_SECURITY_FIRE) {
            return -2; // Compiled for FireSaber but different level requested
        }
    #else
        return -3; // Unsupported SABER_L value
    #endif
    
    // Store security level (for runtime checking)
    current_security_level = security_level;
    is_initialized = 1;
    
    return 0;
}

size_t saber_get_public_key_size(void) {
    return CRYPTO_PUBLICKEYBYTES;
}

size_t saber_get_secret_key_size(void) {
    return CRYPTO_SECRETKEYBYTES;
}

size_t saber_get_ciphertext_size(void) {
    return CRYPTO_CIPHERTEXTBYTES;
}

size_t saber_get_shared_secret_size(void) {
    return CRYPTO_BYTES;
}

int saber_keypair(unsigned char *pk, unsigned char *sk) {
    if (!is_initialized) return -1;
    if (pk == NULL || sk == NULL) return -2;
    return crypto_kem_keypair(pk, sk);
}

int saber_encapsulate(unsigned char *ct, unsigned char *ss, const unsigned char *pk) {
    if (!is_initialized) return -1;
    if (ct == NULL || ss == NULL || pk == NULL) return -2;
    return crypto_kem_enc(ct, ss, pk);
}

int saber_decapsulate(unsigned char *ss, const unsigned char *ct, const unsigned char *sk) {
    if (!is_initialized) return -1;
    if (ss == NULL || ct == NULL || sk == NULL) return -2;
    return crypto_kem_dec(ss, ct, sk);
}
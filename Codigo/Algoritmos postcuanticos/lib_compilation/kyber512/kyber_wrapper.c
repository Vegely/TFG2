#include "kyber_wrapper.h"
#include "params.h"
#include "kem.h"
#include "api.h"
#include "rng.h"

// Current security level
static kyber_security_level_t current_security_level = KYBER_SECURITY_512;

// Static flag to indicate if the library has been initialized
static int is_initialized = 0;

int kyber_init(kyber_security_level_t security_level) {
    // Validate security level
    if (security_level != KYBER_SECURITY_512 && 
        security_level != KYBER_SECURITY_768 && 
        security_level != KYBER_SECURITY_1024) {
        return -1;
    }
    
    // Store security level (for runtime checking)
    current_security_level = security_level;
    is_initialized = 1;
    
    return 0;
}

size_t kyber_get_public_key_size(void) {
    return CRYPTO_PUBLICKEYBYTES;
}

size_t kyber_get_secret_key_size(void) {
    return CRYPTO_SECRETKEYBYTES;
}

size_t kyber_get_ciphertext_size(void) {
    return CRYPTO_CIPHERTEXTBYTES;
}

size_t kyber_get_shared_secret_size(void) {
    return CRYPTO_BYTES;
}

int kyber_keypair(unsigned char *pk, unsigned char *sk) {
    if (!is_initialized) return -1;
    return crypto_kem_keypair(pk, sk);
}

int kyber_encapsulate(unsigned char *ct, unsigned char *ss, const unsigned char *pk) {
    if (!is_initialized) return -1;
    return crypto_kem_enc(ct, ss, pk);
}

int kyber_decapsulate(unsigned char *ss, const unsigned char *ct, const unsigned char *sk) {
    if (!is_initialized) return -1;
    return crypto_kem_dec(ss, ct, sk);
}


/**
 * @file kem.cpp
 * @brief Implementation of api.h
 */

#include "api.h"
#include "hqc.h"
#include "parameters.h"
#include "parsing.h"
#include "shake_ds.h"
#include "fips202.h"
#include "vector.h"
#include <stdint.h>
#include <string.h>
#ifdef VERBOSE
#include <stdio.h>
#endif


/**
 * @brief Keygen of the HQC_KEM IND_CAA2 scheme
 *
 * The public key is composed of the syndrome <b>s</b> as well as the seed used to generate the vector <b>h</b>.
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 * As a technicality, the public key is appended to the secret key in order to respect NIST API.
 *
 * @param[out] pk String containing the public key
 * @param[out] sk String containing the secret key
 * @returns 0 if keygen is successful
 */
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk) {
    #ifdef VERBOSE
        printf("\n\n\n\n### KEYGEN ###");
    #endif

    hqc_pke_keygen(pk, sk);
    return 0;
}



/**
 * @brief Encapsulation of the HQC_KEM IND_CAA2 scheme
 *
 * @param[out] ct String containing the ciphertext
 * @param[out] ss String containing the shared secret
 * @param[in] pk String containing the public key
 * @returns 0 if encapsulation is successful
 */
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk) {
    #ifdef VERBOSE
        printf("\n\n\n\n### ENCAPS ###");
    #endif

    uint8_t theta[SHAKE256_512_BYTES] = {0};
    uint64_t m[VEC_K_SIZE_64] = {0};
    uint64_t u[VEC_N_SIZE_64] = {0};
    uint64_t v[VEC_N1N2_SIZE_64] = {0};
    uint8_t d[SHAKE256_512_BYTES] = {0};
    uint8_t mc[VEC_K_SIZE_BYTES + VEC_N_SIZE_BYTES + VEC_N1N2_SIZE_BYTES] = {0};
    uint64_t salt[SALT_SIZE_64] = {0};
    uint8_t tmp[VEC_K_SIZE_BYTES + SEED_BYTES + SALT_SIZE_BYTES] = {0};
    shake256incctx shake256state;

    // Computing m
    vect_set_random_from_prng(m, VEC_K_SIZE_64);

    // Computing theta
    vect_set_random_from_prng(salt, SALT_SIZE_64);
    memcpy(tmp, m, VEC_K_SIZE_BYTES);
    memcpy(tmp + VEC_K_SIZE_BYTES, pk, SEED_BYTES);
    memcpy(tmp + VEC_K_SIZE_BYTES + SEED_BYTES, salt, SALT_SIZE_BYTES);
    shake256_512_ds(&shake256state, theta, tmp, VEC_K_SIZE_BYTES + SEED_BYTES + SALT_SIZE_BYTES, G_FCT_DOMAIN);

    // Encrypting m
    hqc_pke_encrypt(u, v, m, theta, pk);

    // Computing d
    shake256_512_ds(&shake256state, d, (uint8_t *) m, VEC_K_SIZE_BYTES, H_FCT_DOMAIN);

    // Computing shared secret
    memcpy(mc, m, VEC_K_SIZE_BYTES);
    memcpy(mc + VEC_K_SIZE_BYTES, u, VEC_N_SIZE_BYTES);
    memcpy(mc + VEC_K_SIZE_BYTES + VEC_N_SIZE_BYTES, v, VEC_N1N2_SIZE_BYTES);
    shake256_512_ds(&shake256state, ss, mc, VEC_K_SIZE_BYTES + VEC_N_SIZE_BYTES + VEC_N1N2_SIZE_BYTES, K_FCT_DOMAIN);

    // Computing ciphertext
    hqc_ciphertext_to_string(ct, u, v, d, salt);

    #ifdef VERBOSE
        printf("\n\npk: "); for(int i = 0 ; i < PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
        printf("\n\nm: "); vect_print(m, VEC_K_SIZE_BYTES);
        printf("\n\ntheta: "); for(int i = 0 ; i < SHAKE256_512_BYTES ; ++i) printf("%02x", theta[i]);
        printf("\n\nd: "); for(int i = 0 ; i < SHAKE256_512_BYTES ; ++i) printf("%02x", d[i]);
        printf("\n\nciphertext: "); for(int i = 0 ; i < CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
        printf("\n\nsecret 1: "); for(int i = 0 ; i < SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
    #endif

    return 0;
}



/**
 * @brief Decapsulation of the HQC_KEM IND_CAA2 scheme
 *
 * @param[out] ss String containing the shared secret
 * @param[in] ct String containing the cipĥertext
 * @param[in] sk String containing the secret key
 * @returns 0 if decapsulation is successful, -1 otherwise
 */
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk) {
    #ifdef VERBOSE
        printf("\n\n\n\n### DECAPS ###");
    #endif

    uint8_t result;
    uint64_t u[VEC_N_SIZE_64] = {0};
    uint64_t v[VEC_N1N2_SIZE_64] = {0};
    uint8_t d[SHAKE256_512_BYTES] = {0};
    uint8_t pk[PUBLIC_KEY_BYTES] = {0};
    uint64_t m[VEC_K_SIZE_64] = {0};
    uint8_t theta[SHAKE256_512_BYTES] = {0};
    uint64_t u2[VEC_N_SIZE_64] = {0};
    uint64_t v2[VEC_N1N2_SIZE_64] = {0};
    uint8_t d2[SHAKE256_512_BYTES] = {0};
    uint8_t mc[VEC_K_SIZE_BYTES + VEC_N_SIZE_BYTES + VEC_N1N2_SIZE_BYTES] = {0};
    uint64_t salt[SALT_SIZE_64] = {0};
    uint8_t tmp[VEC_K_SIZE_BYTES + SALT_SIZE_BYTES + SEED_BYTES] = {0};
    shake256incctx shake256state;

    // Retrieving u, v and d from ciphertext
    hqc_ciphertext_from_string(u, v , d, salt, ct);

    // Retrieving pk from sk
    memcpy(pk, sk + SEED_BYTES, PUBLIC_KEY_BYTES);

    // Decryting
    hqc_pke_decrypt(m, u, v, sk);

    // Computing theta
    memcpy(tmp, m, VEC_K_SIZE_BYTES);
    memcpy(tmp + VEC_K_SIZE_BYTES, pk, SEED_BYTES);
    memcpy(tmp + VEC_K_SIZE_BYTES + SEED_BYTES, salt, SALT_SIZE_BYTES);
    shake256_512_ds(&shake256state, theta, tmp, VEC_K_SIZE_BYTES + SEED_BYTES + SALT_SIZE_BYTES, G_FCT_DOMAIN);

    // Encrypting m'
    hqc_pke_encrypt(u2, v2, m, theta, pk);

    // Computing d'
    shake256_512_ds(&shake256state, d2, (uint8_t *) m, VEC_K_SIZE_BYTES, H_FCT_DOMAIN);

    // Computing shared secret
    memcpy(mc, m, VEC_K_SIZE_BYTES);
    memcpy(mc + VEC_K_SIZE_BYTES, u, VEC_N_SIZE_BYTES);
    memcpy(mc + VEC_K_SIZE_BYTES + VEC_N_SIZE_BYTES, v, VEC_N1N2_SIZE_BYTES);
    shake256_512_ds(&shake256state, ss, mc, VEC_K_SIZE_BYTES + VEC_N_SIZE_BYTES + VEC_N1N2_SIZE_BYTES, K_FCT_DOMAIN);

    // Abort if c != c' or d != d'
    result = vect_compare((uint8_t *)u, (uint8_t *)u2, VEC_N_SIZE_BYTES);
    result |= vect_compare((uint8_t *)v, (uint8_t *)v2, VEC_N1N2_SIZE_BYTES);
    result |= vect_compare(d, d2, SHAKE256_512_BYTES);

    result = (uint8_t) (-((int16_t) result) >> 15);

    for (size_t i = 0 ; i < SHARED_SECRET_BYTES ; i++) {
        ss[i] &= ~result;
    }

    #ifdef VERBOSE
        printf("\n\npk: "); for(int i = 0 ; i < PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
        printf("\n\nsk: "); for(int i = 0 ; i < SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
        printf("\n\nciphertext: "); for(int i = 0 ; i < CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
        printf("\n\nm: "); vect_print(m, VEC_K_SIZE_BYTES);
        printf("\n\ntheta: "); for(int i = 0 ; i < SHAKE256_512_BYTES ; ++i) printf("%02x", theta[i]);
        printf("\n\n\n# Checking Ciphertext- Begin #");
        printf("\n\nu2: "); vect_print(u2, VEC_N_SIZE_BYTES);
        printf("\n\nv2: "); vect_print(v2, VEC_N1N2_SIZE_BYTES);
        printf("\n\nd2: "); for(int i = 0 ; i < SHAKE256_512_BYTES ; ++i) printf("%02x", d2[i]);
        printf("\n\n# Checking Ciphertext - End #\n");
    #endif

    return -(result & 1);
}

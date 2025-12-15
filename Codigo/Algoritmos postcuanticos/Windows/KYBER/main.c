#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "kyber_wrapper.h"

// Helper to print bytes in hex
void print_hex(const char* label, const unsigned char* data, size_t len) {
    printf("%-25s: ", label);
    // Printing first 16 bytes only to avoid console bloat. 
    // Change limit to 'len' to see the whole key.
    size_t limit = (len > 16) ? 16 : len;
    for (size_t i = 0; i < limit; i++) {
        printf("%02X", data[i]);
    }
    if (len > 16) printf("...");
    printf("\n");
}

int main(void) {
    kyber_init(KYBER_SECURITY_1024);

    // --- Allocations ---
    size_t pk_len = kyber_get_public_key_size();
    size_t sk_len = kyber_get_secret_key_size();
    size_t ct_len = kyber_get_ciphertext_size();
    size_t ss_len = kyber_get_shared_secret_size();

    unsigned char* pk = malloc(pk_len);
    unsigned char* sk = malloc(sk_len);
    unsigned char* ct = malloc(ct_len);
    unsigned char* ss_orig = malloc(ss_len);
    unsigned char* ss_decap = malloc(ss_len);

    if (!pk || !sk || !ct || !ss_orig || !ss_decap) return 1;

    printf("Kyber Operations\n\n");

    // 1. Keypair Creation
    kyber_keypair(pk, sk);
    print_hex("Public Key", pk, pk_len);
    print_hex("Secret Key", sk, sk_len);

    // 2. Encapsulation
    kyber_encapsulate(ct, ss_orig, pk);
    print_hex("Ciphertext", ct, ct_len);
    print_hex("Shared Secret (Orig)", ss_orig, ss_len);

    // 3. Decapsulation
    kyber_decapsulate(ss_decap, ct, sk);
    print_hex("Shared Secret (Decap)", ss_decap, ss_len);

    // Cleanup
    free(pk); free(sk); free(ct); free(ss_orig); free(ss_decap);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hqc_wrapper.h"

#define TEST_COUNT 1000

void print_hex(const char *label, const unsigned char *data, size_t len) {
    printf("%s (%zu bytes):\n", label, len);
    for (size_t i = 0; i < len; ++i) {
        printf("%02X", data[i]);
        if ((i + 1) % 32 == 0) printf("\n");
    }
    if (len % 32 != 0) printf("\n");
    printf("\n");
}

int main(void) {
    if (hqc_init(HQC_SECURITY_256) != 0) {
        fprintf(stderr, "HQC init failed\n");
        return 1;
    }

    size_t pk_size = hqc_get_public_key_size();
    size_t sk_size = hqc_get_secret_key_size();
    size_t ct_size = hqc_get_ciphertext_size();
    size_t ss_size = hqc_get_shared_secret_size();

    printf("HQC Sizes:\n");
    printf("  Public Key Size     : %zu bytes\n", pk_size);
    printf("  Secret Key Size     : %zu bytes\n", sk_size);
    printf("  Ciphertext Size     : %zu bytes\n", ct_size);
    printf("  Shared Secret Size  : %zu bytes\n\n", ss_size);

    unsigned char *pk = malloc(pk_size);
    unsigned char *sk = malloc(sk_size);
    unsigned char *ct = malloc(ct_size);
    unsigned char *ss1 = malloc(ss_size);
    unsigned char *ss2 = malloc(ss_size);

    if (!pk || !sk || !ct || !ss1 || !ss2) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    if (hqc_keypair(pk, sk) != 0) {
        fprintf(stderr, "Keypair generation failed\n");
        return 1;
    }

    print_hex("Public Key", pk, pk_size);
    print_hex("Secret Key", sk, sk_size);

    // Allocate memory for storing all shared secrets
    unsigned char **shared_secrets_sender = malloc(TEST_COUNT * sizeof(unsigned char *));
    unsigned char **shared_secrets_receiver = malloc(TEST_COUNT * sizeof(unsigned char *));
    for (int i = 0; i < TEST_COUNT; ++i) {
        shared_secrets_sender[i] = malloc(ss_size);
        shared_secrets_receiver[i] = malloc(ss_size);
    }

    int mismatch_found = 0;

    for (int i = 0; i < TEST_COUNT; ++i) {
        if (hqc_encapsulate(ct, ss1, pk) != 0) {
            fprintf(stderr, "Encapsulation failed at test %d\n", i);
            return 1;
        }

        if (hqc_decapsulate(ss2, ct, sk) != 0) {
            fprintf(stderr, "Decapsulation failed at test %d\n", i);
            return 1;
        }

        memcpy(shared_secrets_sender[i], ss1, ss_size);
        memcpy(shared_secrets_receiver[i], ss2, ss_size);

        if (memcmp(ss1, ss2, ss_size) != 0) {
            printf("Mismatch at test %d\n", i);
            mismatch_found = 1;
        }
    }

    printf("\n=== Shared Secrets Comparison ===\n");
    if (!mismatch_found) {
        printf("All %d shared secrets matched correctly.\n", TEST_COUNT);
    }

    // Check if any shared secrets unexpectedly changed (should be unique)
    for (int i = 0; i < TEST_COUNT; ++i) {
        for (int j = i + 1; j < TEST_COUNT; ++j) {
            if (memcmp(shared_secrets_sender[i], shared_secrets_sender[j], ss_size) == 0) {
                printf("Warning: Duplicate shared secrets found at indices %d and %d\n", i, j);
            }
        }
    }

    free(pk);
    free(sk);
    free(ct);
    free(ss1);
    free(ss2);
    for (int i = 0; i < TEST_COUNT; ++i) {
        free(shared_secrets_sender[i]);
        free(shared_secrets_receiver[i]);
    }
    free(shared_secrets_sender);
    free(shared_secrets_receiver);

    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
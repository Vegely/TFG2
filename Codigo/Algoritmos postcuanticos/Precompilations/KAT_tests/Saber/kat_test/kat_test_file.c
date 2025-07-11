//
// SaberKAT.c - Known Answer Test implementation for SABER using the wrapper interface
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "saber_wrapper.h"
#include "rng.h"

// Define constants for the KAT test
#define MAX_MARKER_LEN       50
#define KAT_SUCCESS          0
#define KAT_FILE_OPEN_ERROR -1
#define KAT_DATA_ERROR      -3
#define KAT_CRYPTO_FAILURE  -4

// Helper function prototypes
int FindMarker(FILE *infile, const char *marker);
int ReadHex(FILE *infile, unsigned char *A, int Length, char *str);
void fprintBstr(FILE *fp, char *S, unsigned char *A, unsigned long long L);

int main(int argc, char *argv[])
{
    // Default to FireSaber if not specified
    saber_security_level_t security_level = SABER_SECURITY_FIRE;
    
    // Parse command-line arguments for security level
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--light") == 0 || strcmp(argv[i], "-light") == 0 || strcmp(argv[i], "--2") == 0) {
            security_level = SABER_SECURITY_LIGHT;
        } else if (strcmp(argv[i], "--saber") == 0 || strcmp(argv[i], "-saber") == 0 || strcmp(argv[i], "--3") == 0) {
            security_level = SABER_SECURITY_SABER;
        } else if (strcmp(argv[i], "--fire") == 0 || strcmp(argv[i], "-fire") == 0 || strcmp(argv[i], "--4") == 0) {
            security_level = SABER_SECURITY_FIRE;
        } else {
            printf("Unknown argument: %s\n", argv[i]);
            printf("Usage: %s [--light|--saber|--fire] or [--2|--3|--4]\n", argv[0]);
            return 1;
        }
    }
    
    // Initialize the SABER library with the chosen security level
    if (saber_init(security_level) != 0) {
        printf("Failed to initialize SABER with security level %d\n", security_level);
        printf("Note: Make sure the compiled SABER_L matches the requested security level\n");
        return KAT_CRYPTO_FAILURE;
    }
    
    // Get key and ciphertext sizes based on chosen security parameters
    size_t public_key_size = saber_get_public_key_size();
    size_t secret_key_size = saber_get_secret_key_size();
    size_t ciphertext_size = saber_get_ciphertext_size();
    size_t shared_secret_size = saber_get_shared_secret_size();
    
    char fn_req[100], fn_rsp[100];
    FILE *fp_req, *fp_rsp;
    unsigned char seed[48];
    unsigned char entropy_input[48];
    unsigned char *ct, *ss, *ss1;
    int count;
    int done;
    unsigned char *pk, *sk;
    int ret_val;
    
    // Get the algorithm name for file naming
    const char* algo_name;
    switch(security_level) {
        case SABER_SECURITY_LIGHT:
            algo_name = "LightSaber";
            break;
        case SABER_SECURITY_SABER:
            algo_name = "Saber";
            break;
        case SABER_SECURITY_FIRE:
            algo_name = "FireSaber";
            break;
        default:
            algo_name = "UnknownSaber";
            break;
    }

    // Allocate memory for the keys, ciphertext, and shared secrets
    pk = (unsigned char*)malloc(public_key_size);
    sk = (unsigned char*)malloc(secret_key_size);
    ct = (unsigned char*)malloc(ciphertext_size);
    ss = (unsigned char*)malloc(shared_secret_size);
    ss1 = (unsigned char*)malloc(shared_secret_size);
    
    if (!pk || !sk || !ct || !ss || !ss1) {
        printf("Memory allocation failed\n");
        return -1;
    }

    // Create filenames based on security level
    sprintf(fn_req, "%sKAT_%d.req", algo_name, security_level);
    if ((fp_req = fopen(fn_req, "w")) == NULL) {
        printf("Couldn't open <%s> for write\n", fn_req);
        return KAT_FILE_OPEN_ERROR;
    }
    
    sprintf(fn_rsp, "%sKAT_%d.rsp", algo_name, security_level);
    if ((fp_rsp = fopen(fn_rsp, "w")) == NULL) {
        printf("Couldn't open <%s> for write\n", fn_rsp);
        return KAT_FILE_OPEN_ERROR;
    }

    // Initialize entropy input
    for (int i = 0; i < 48; i++)
        entropy_input[i] = i;

    // Initialize the random number generator for KAT testing
    randombytes_init(entropy_input, NULL, 256);

    // Generate the request file with 100 test vectors
    for (int i = 0; i < 100; i++) {
        fprintf(fp_req, "count = %d\n", i);
        
        // Generate a random seed for this test case
        randombytes(seed, 48);
        
        fprintBstr(fp_req, "seed = ", seed, 48);
        fprintf(fp_req, "pk =\n");
        fprintf(fp_req, "sk =\n");
        fprintf(fp_req, "ct =\n");
        fprintf(fp_req, "ss =\n\n");
    }
    fclose(fp_req);

    // Create the RESPONSE file based on what's in the REQUEST file
    if ((fp_req = fopen(fn_req, "r")) == NULL) {
        printf("Couldn't open <%s> for read\n", fn_req);
        return KAT_FILE_OPEN_ERROR;
    }

    fprintf(fp_rsp, "# %s KAT Responses\n\n", algo_name);
    done = 0;
    do {
        if (FindMarker(fp_req, "count = "))
            fscanf(fp_req, "%d", &count);
        else {
            done = 1;
            break;
        }
        fprintf(fp_rsp, "count = %d\n", count);

        if (!ReadHex(fp_req, seed, 48, "seed = ")) {
            printf("ERROR: unable to read 'seed' from <%s>\n", fn_req);
            return KAT_DATA_ERROR;
        }
        fprintBstr(fp_rsp, "seed = ", seed, 48);

        // Initialize the random number generator with the seed
        randombytes_init(seed, NULL, 256);

        // Generate the public/private keypair
        if ((ret_val = saber_keypair(pk, sk)) != 0) {
            printf("saber_keypair returned <%d>\n", ret_val);
            return KAT_CRYPTO_FAILURE;
        }
        fprintBstr(fp_rsp, "pk = ", pk, public_key_size);
        fprintBstr(fp_rsp, "sk = ", sk, secret_key_size);

        // Perform encapsulation
        if ((ret_val = saber_encapsulate(ct, ss, pk)) != 0) {
            printf("saber_encapsulate returned <%d>\n", ret_val);
            return KAT_CRYPTO_FAILURE;
        }
        fprintBstr(fp_rsp, "ct = ", ct, ciphertext_size);
        fprintBstr(fp_rsp, "ss = ", ss, shared_secret_size);

        fprintf(fp_rsp, "\n");

        // Perform decapsulation and verify the results
        if ((ret_val = saber_decapsulate(ss1, ct, sk)) != 0) {
            printf("saber_decapsulate returned <%d>\n", ret_val);
            return KAT_CRYPTO_FAILURE;
        }

        // Verify that encapsulation and decapsulation produced the same shared secret
        if (memcmp(ss, ss1, shared_secret_size)) {
            printf("saber_decapsulate returned bad shared secret value\n");
            return KAT_CRYPTO_FAILURE;
        }

    } while (!done);

    // Clean up
    fclose(fp_req);
    fclose(fp_rsp);
    free(pk);
    free(sk);
    free(ct);
    free(ss);
    free(ss1);

    printf("KAT tests completed successfully for %s\n", algo_name);
    printf("Files generated: %s and %s\n", fn_req, fn_rsp);
    printf("Key sizes: pk=%zu, sk=%zu, ct=%zu, ss=%zu\n", 
           public_key_size, secret_key_size, ciphertext_size, shared_secret_size);
    
    return KAT_SUCCESS;
}

//
// Find a marker in the input file
//
int FindMarker(FILE *infile, const char *marker)
{
    char line[MAX_MARKER_LEN];
    int i, len;
    int curr_line;

    len = (int)strlen(marker);
    if (len > MAX_MARKER_LEN-1)
        len = MAX_MARKER_LEN-1;

    for (i = 0; i < len; i++) {
        curr_line = fgetc(infile);
        line[i] = curr_line;
        if (curr_line == EOF)
            return 0;
    }
    line[len] = '\0';

    while (1) {
        if (!strncmp(line, marker, len))
            return 1;

        for (i = 0; i < len-1; i++)
            line[i] = line[i+1];
        curr_line = fgetc(infile);
        line[len-1] = curr_line;
        if (curr_line == EOF)
            return 0;
        line[len] = '\0';
    }

    // shouldn't get here
    return 0;
}

//
// Read a hex string from the input file
//
int ReadHex(FILE *infile, unsigned char *A, int Length, char *str)
{
    int i, ch, started;
    unsigned char ich;

    if (Length == 0) {
        A[0] = 0x00;
        return 1;
    }
    memset(A, 0x00, Length);
    started = 0;
    if (FindMarker(infile, str))
        while ((ch = fgetc(infile)) != EOF) {
            if (!isxdigit(ch)) {
                if (!started) {
                    if (ch == '\n')
                        break;
                    else
                        continue;
                }
                else
                    break;
            }
            started = 1;
            if ((ch >= '0') && (ch <= '9'))
                ich = ch - '0';
            else if ((ch >= 'A') && (ch <= 'F'))
                ich = ch - 'A' + 10;
            else if ((ch >= 'a') && (ch <= 'f'))
                ich = ch - 'a' + 10;
            else // shouldn't ever get here
                ich = 0;

            for (i = 0; i < Length-1; i++)
                A[i] = (A[i] << 4) | (A[i+1] >> 4);
            A[Length-1] = (A[Length-1] << 4) | ich;
        }
    else
        return 0;

    return 1;
}

//
// Print a binary string as hex
//
void fprintBstr(FILE *fp, char *S, unsigned char *A, unsigned long long L)
{
    unsigned long long i;

    fprintf(fp, "%s", S);

    for (i = 0; i < L; i++)
        fprintf(fp, "%02X", A[i]);

    if (L == 0)
        fprintf(fp, "00");

    fprintf(fp, "\n");
}
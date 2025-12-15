#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saber_wrapper.h"
#include <stdint.h>
#include <windows.h> 

#define ITERATIONS 100


/*int main(void) {
    if (saber_init(SABER_SECURITY_FIRE) != 0) {
        fprintf(stderr, "SABER init failed\n");
        return 1;
    }

    size_t pk_size = saber_get_public_key_size();
    size_t sk_size = saber_get_secret_key_size();
    size_t ct_size = saber_get_ciphertext_size();
    size_t ss_size = saber_get_shared_secret_size();

    printf("SABER Sizes:\n");
    printf("  Public Key Size     : %zu bytes\n", pk_size);
    printf("  Secret Key Size     : %zu bytes\n", sk_size);
    printf("  Ciphertext Size     : %zu bytes\n", ct_size);
    printf("  Shared Secret Size  : %zu bytes\n\n", ss_size);

    unsigned char* pk = malloc(pk_size);
    unsigned char* sk = malloc(sk_size);
    unsigned char* ct = malloc(ct_size);
    unsigned char* ss = malloc(ss_size);


    if (!pk || !sk || !ct || !ss) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    unsigned long long start_cycles, end_cycles, cycle_count;
    unsigned long long total_cycles = 0;
    printf("Measuring function performance over %d iterations...\n\n", ITERATIONS);
    printf("===KEYPAIR===...\n\n");

    for (int i = 0; i < ITERATIONS; i++) {

        // A. Capture Start Cycles
        // We use a "Fence" (optional but recommended) to prevent the CPU 
        // from re-ordering instructions for optimization.
        _mm_lfence();
        start_cycles = __rdtsc();
        _mm_lfence();

        // B. Run the function
        if (saber_keypair(pk, sk) != 0) fprintf(stderr, "Keypair generation failed\n");

        // C. Capture End Cycles
        _mm_lfence();
        end_cycles = __rdtsc();
        _mm_lfence();

        // D. Calculate difference
        cycle_count = end_cycles - start_cycles;
        total_cycles += cycle_count;

        printf("Run %d: %llu cycles\n", i + 1, cycle_count);
    }

    // E. Calculate Average
    double average = (double)total_cycles / ITERATIONS;

    printf("\n----------------------------\n");
    printf("Total Cycles: %llu\n", total_cycles);
    printf("Average Cycles: %.2f\n", average);
    printf("===Encapsulation===...\n\n");
    total_cycles = 0;
    for (int i = 0; i < ITERATIONS; i++) {

        // A. Capture Start Cycles
        // We use a "Fence" (optional but recommended) to prevent the CPU 
        // from re-ordering instructions for optimization.
        _mm_lfence();
        start_cycles = __rdtsc();
        _mm_lfence();

        // B. Run the function
        if (saber_encapsulate(ct, ss, pk) != 0) fprintf(stderr, "Encapsulation failed\n");

        // C. Capture End Cycles
        _mm_lfence();
        end_cycles = __rdtsc();
        _mm_lfence();

        // D. Calculate difference
        cycle_count = end_cycles - start_cycles;
        total_cycles += cycle_count;

        printf("Run %d: %llu cycles\n", i + 1, cycle_count);
    }
    // E. Calculate Average
    average = (double)total_cycles / ITERATIONS;

    printf("\n----------------------------\n");
    printf("Total Cycles: %llu\n", total_cycles);
    printf("Average Cycles: %.2f\n", average);
    printf("===Encapsulation===...\n\n");

    printf("===Decapsulation===...\n\n");
    total_cycles = 0;
    for (int i = 0; i < ITERATIONS; i++) {

        // A. Capture Start Cycles
        // We use a "Fence" (optional but recommended) to prevent the CPU 
        // from re-ordering instructions for optimization.
        _mm_lfence();
        start_cycles = __rdtsc();
        _mm_lfence();

        // B. Run the function
        if (saber_decapsulate(ss, ct, sk) != 0)  fprintf(stderr, "Decapsulation failed\n");

        // C. Capture End Cycles
        _mm_lfence();
        end_cycles = __rdtsc();
        _mm_lfence();

        // D. Calculate difference
        cycle_count = end_cycles - start_cycles;
        total_cycles += cycle_count;

        printf("Run %d: %llu cycles\n", i + 1, cycle_count);
    }
    // E. Calculate Average
    average = (double)total_cycles / ITERATIONS;

    printf("\n----------------------------\n");
    printf("Total Cycles: %llu\n", total_cycles);
    printf("Average Cycles: %.2f\n", average);
    printf("===Encapsulation===...\n\n");
    free(pk);
    free(sk);
    free(ct);
    free(ss);



    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}*/

/*
#define STACK_SIZE (256 * 1024) 
#define CANARY 0xAA

// Global pointer to track where our "trap" memory is
volatile uint8_t* g_stack_trap = NULL;

// 1. THE PAINTER
// This function claims stack space, fills it, and returns.
// By returning, it marks the memory as "free" but leaves the 0xAA bytes there.
void paint_stack(void) {
    volatile uint8_t buffer[STACK_SIZE];

    // Save the address so we can check it later
    // (Technically unsafe in normal code, but perfect for this specific hack)
    g_stack_trap = &buffer[0];

    for (int i = 0; i < STACK_SIZE; i++) {
        buffer[i] = CANARY;
    }
    // Returns, releasing 'buffer' for the next function to reuse
}

// 2. THE CHECKER
// Looks at the memory previously occupied by paint_stack
size_t check_stack_depth(void) {
    size_t depth = 0;
    for (int i = 0; i < STACK_SIZE; i++) {
        // We look for the first byte that ISN'T 0xAA
        // Since stack grows down, the "deepest" usage is at the lowest index
        if (g_stack_trap[i] != CANARY) {
            // Found a modified byte!
            // Calculate how deep the stack went (from the top of buffer down)
            depth = STACK_SIZE - i;
            break;
        }
    }
    return depth;
}

int main(void) {
    // Buffers for crypto data
    unsigned char* pk = malloc(saber_get_public_key_size());
    unsigned char* sk = malloc(saber_get_secret_key_size());
    unsigned char* ct = malloc(saber_get_ciphertext_size());
    unsigned char* ss = malloc(saber_get_shared_secret_size());

    saber_init(SABER_SECURITY_FIRE);

    printf("=== Corrected Stack Analysis ===\n\n");

    // TEST 1: KEYPAIR
    paint_stack();           // 1. Fill memory with 0xAA
    saber_keypair(pk, sk);     // 2. Run function (overwrites 0xAA)
    size_t d = check_stack_depth(); // 3. Measure
    printf("Keypair Depth      : %zu bytes\n", d);

    // TEST 2: ENCAPSULATION
    paint_stack();           // Reset memory
    saber_encapsulate(ct, ss, pk);
    d = check_stack_depth();
    printf("Encapsulation Depth: %zu bytes\n", d);

    // TEST 3: DECAPSULATION
    paint_stack();           // Reset memory
    saber_decapsulate(ss, ct, sk);
    d = check_stack_depth();
    printf("Decapsulation Depth: %zu bytes\n", d);

    getchar();
    return 0;
}*/

int main(void) {
    if (saber_init(SABER_SECURITY_FIRE) != 0) {
        fprintf(stderr, "HQC init failed\n");
        return 1;
    }

    size_t pk_size = saber_get_public_key_size();
    size_t sk_size = saber_get_secret_key_size();
    size_t ct_size = saber_get_ciphertext_size();
    size_t ss_size = saber_get_shared_secret_size();

    printf("Saber Sizes:\n");
    printf("  Public Key Size      : %zu bytes\n", pk_size);
    printf("  Secret Key Size      : %zu bytes\n", sk_size);
    printf("  Ciphertext Size      : %zu bytes\n", ct_size);
    printf("  Shared Secret Size   : %zu bytes\n\n", ss_size);

    unsigned char* pk = malloc(pk_size);
    unsigned char* sk = malloc(sk_size);
    unsigned char* ct = malloc(ct_size);
    unsigned char* ss = malloc(ss_size);

    if (!pk || !sk || !ct || !ss) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Windows Timing Variables
    LARGE_INTEGER frequency; // Ticks per second
    LARGE_INTEGER start, current;
    double elapsed_seconds;
    unsigned long long count;

    // Get the CPU frequency for timing
    QueryPerformanceFrequency(&frequency);

    printf("--------------------------------------------------\n");
    printf("Starting Performance Benchmark (1 second per op)...\n");
    printf("--------------------------------------------------\n\n");


    // ===========================
    // 1. KEYPAIR GENERATION
    // ===========================
    printf("Benchmarking Keypair... ");
    count = 0;
    QueryPerformanceCounter(&start); // Start Timer

    do {
        if (saber_keypair(pk, sk) != 0) {
            fprintf(stderr, "Keypair generation failed\n");
            break;
        }
        count++;
        QueryPerformanceCounter(&current);
        // Calculate elapsed time in seconds
        elapsed_seconds = (double)(current.QuadPart - start.QuadPart) / frequency.QuadPart;
    } while (elapsed_seconds < 1.0);

    printf("Done.\n");
    printf("-> Total Iterations: %llu\n", count);
    printf("-> Throughput      : %.2f ops/sec\n\n", count / elapsed_seconds);


    // ===========================
    // 2. ENCAPSULATION
    // ===========================
    // Note: We reuse the pk generated in the last iteration of the previous step
    printf("Benchmarking Encapsulation... ");
    count = 0;
    QueryPerformanceCounter(&start); // Start Timer

    do {
        if (saber_encapsulate(ct, ss, pk) != 0) {
            fprintf(stderr, "Encapsulation failed\n");
            break;
        }
        count++;
        QueryPerformanceCounter(&current);
        elapsed_seconds = (double)(current.QuadPart - start.QuadPart) / frequency.QuadPart;
    } while (elapsed_seconds < 1.0);

    printf("Done.\n");
    printf("-> Total Iterations: %llu\n", count);
    printf("-> Throughput      : %.2f ops/sec\n\n", count / elapsed_seconds);


    // ===========================
    // 3. DECAPSULATION
    // ===========================
    // Note: We reuse the ct and sk from the previous steps
    printf("Benchmarking Decapsulation... ");
    count = 0;
    QueryPerformanceCounter(&start); // Start Timer

    do {
        if (saber_decapsulate(ss, ct, sk) != 0) {
            fprintf(stderr, "Decapsulation failed\n");
            break;
        }
        count++;
        QueryPerformanceCounter(&current);
        elapsed_seconds = (double)(current.QuadPart - start.QuadPart) / frequency.QuadPart;
    } while (elapsed_seconds < 1.0);

    printf("Done.\n");
    printf("-> Total Iterations: %llu\n", count);
    printf("-> Throughput      : %.2f ops/sec\n\n", count / elapsed_seconds);


    // Cleanup
    free(pk);
    free(sk);
    free(ct);
    free(ss);

    printf("Benchmark complete. Press Enter to exit...");
    getchar();
    return 0;
}
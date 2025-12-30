#include "AlgorithmTester.h"
#include <iostream>
#include <iomanip>
#include <intrin.h> 

AlgorithmTester::AlgorithmTester(KeyExchange* algorithm) : algo_(algorithm), stack_trap_(nullptr) {
    QueryPerformanceFrequency(&frequency_);
    resizeBuffers();
}

void AlgorithmTester::resizeBuffers() {
    pk_buf.resize(algo_->get_public_key_size());
    sk_buf.resize(algo_->get_secret_key_size());
    ct_buf.resize(algo_->get_ciphertext_size());
    ss_buf.resize(algo_->get_shared_secret_size());
}

__declspec(noinline) void AlgorithmTester::paintStack() {
    volatile uint8_t buffer[STACK_SIZE];
    stack_trap_ = &buffer[0];
    for (int i = 0; i < STACK_SIZE; i++) {
        buffer[i] = CANARY;
    }
}

size_t AlgorithmTester::checkStackDepth() {
    size_t depth = 0;
    for (int i = 0; i < STACK_SIZE; i++) {
        if (stack_trap_[i] != CANARY) {
            depth = STACK_SIZE - i;
            break;
        }
    }
    return depth;
}

void AlgorithmTester::runStackAnalysis() {
    std::cout << "=== Stack Analysis ===\n";

    // Keypair 
    paintStack();
    algo_->keypair(pk_buf.data(), sk_buf.data());
    std::cout << "Keypair Depth      : " << checkStackDepth() << " bytes\n";

    // Encapsulation 
    paintStack();
    algo_->encapsulate(ct_buf.data(), ss_buf.data(), pk_buf.data());
    std::cout << "Encapsulation Depth: " << checkStackDepth() << " bytes\n";

    // Decapsulation
    paintStack();
    algo_->decapsulate(ss_buf.data(), ct_buf.data(), sk_buf.data());
    std::cout << "Decapsulation Depth: " << checkStackDepth() << " bytes\n\n";
}

void AlgorithmTester::runCycleAnalysis() {
    const int ITERATIONS = 100;
    unsigned long long start, end, total = 0;

    std::cout << "=== CPU Cycle Analysis (" << ITERATIONS << " runs) ===\n";

    // Keypair
    total = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        _mm_lfence();
        start = __rdtsc();
        _mm_lfence();

        algo_->keypair(pk_buf.data(), sk_buf.data());

        _mm_lfence();
        end = __rdtsc();
        _mm_lfence();
        total += (end - start);
    }
    std::cout << "Keypair Avg Cycles: " << (total / ITERATIONS) << "\n";

    // Encapsulation
    total = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        _mm_lfence();
        start = __rdtsc();
        _mm_lfence();

        algo_->encapsulate(ct_buf.data(), ss_buf.data(), pk_buf.data());

        _mm_lfence();
        end = __rdtsc();
        _mm_lfence();
        total += (end - start);
    }
    std::cout << "Encaps Avg Cycles : " << (total / ITERATIONS) << "\n";

    // Decapsulation
    total = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        _mm_lfence();
        start = __rdtsc();
        _mm_lfence();

        algo_->decapsulate(ss_buf.data(), ct_buf.data(), sk_buf.data());

        _mm_lfence();
        end = __rdtsc();
        _mm_lfence();
        total += (end - start);
    }
    std::cout << "Decaps Avg Cycles : " << (total / ITERATIONS) << "\n\n";
}

void AlgorithmTester::runThroughputAnalysis() {
    LARGE_INTEGER start, current;
    double elapsed;
    unsigned long long count;

    std::cout << "=== Throughput Analysis (1 second per op) ===\n";

    // Keypair 
    count = 0;
    QueryPerformanceCounter(&start);
    do {
        algo_->keypair(pk_buf.data(), sk_buf.data());
        count++;
        QueryPerformanceCounter(&current);
        elapsed = (double)(current.QuadPart - start.QuadPart) / frequency_.QuadPart;
    } while (elapsed < 1.0);
    std::cout << "Keypair: " << std::fixed << std::setprecision(2) << (count / elapsed) << " ops/sec\n";

    // Encapsulation
    count = 0;
    QueryPerformanceCounter(&start);
    do {
        algo_->encapsulate(ct_buf.data(), ss_buf.data(), pk_buf.data());
        count++;
        QueryPerformanceCounter(&current);
        elapsed = (double)(current.QuadPart - start.QuadPart) / frequency_.QuadPart;
    } while (elapsed < 1.0);
    std::cout << "Encaps : " << std::fixed << std::setprecision(2) << (count / elapsed) << " ops/sec\n";

    // Decapsulation
    count = 0;
    QueryPerformanceCounter(&start);
    do {
        algo_->decapsulate(ss_buf.data(), ct_buf.data(), sk_buf.data());
        count++;
        QueryPerformanceCounter(&current);
        elapsed = (double)(current.QuadPart - start.QuadPart) / frequency_.QuadPart;
    } while (elapsed < 1.0);
    std::cout << "Decaps : " << std::fixed << std::setprecision(2) << (count / elapsed) << " ops/sec\n\n";
}
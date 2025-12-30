#ifndef ALGORITHM_TESTER_H
#define ALGORITHM_TESTER_H

#include "keyexhange.h" 
#include <vector>
#include <windows.h>
#include <string>

class AlgorithmTester {
public:
    AlgorithmTester(KeyExchange* algorithm);

    void runStackAnalysis();
    void runCycleAnalysis();
    void runThroughputAnalysis();

private:
    KeyExchange* algo_;
    LARGE_INTEGER frequency_;

    // Stack analysis state
    volatile uint8_t* stack_trap_;
    static const int STACK_SIZE = 256 * 1024;
    static const int CANARY = 0xAA;

    // Helpers
    void paintStack();
    size_t checkStackDepth();

    // Memory management helpers
    std::vector<unsigned char> pk_buf, sk_buf, ct_buf, ss_buf;
    void resizeBuffers();
};

#endif // ALGORITHM_TESTER_H
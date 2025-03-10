#include "CircularBuffer.h"
#include <iostream>
#include <string>

void runCircularBufferTests() {
    // Test: Push and Pop
    {
        CircularBuffer buffer(3);
        std::string value;

        // Test push
        if (buffer.push("A") && buffer.push("B") && buffer.push("C")) {
            std::cout << "[PASSED] testPush\n";
        }
        else {
            std::cout << "[FAILED] testPush\n";
        }

        // Test push overflow
        if (!buffer.push("D")) {
            std::cout << "[PASSED] testPushOverflow\n";
        }
        else {
            std::cout << "[FAILED] testPushOverflow\n";
        }

        // Test pop
        if (buffer.pop(value) && value == "A" && buffer.pop(value) && value == "B" && buffer.pop(value) && value == "C") {
            std::cout << "[PASSED] testPop\n";
        }
        else {
            std::cout << "[FAILED] testPop\n";
        }

        // Test pop underflow
        if (!buffer.pop(value)) {
            std::cout << "[PASSED] testPopUnderflow\n";
        }
        else {
            std::cout << "[FAILED] testPopUnderflow\n";
        }
    }

    // Test: Get Size and Count
    {
        CircularBuffer buffer(3);

        if (buffer.getSize() == 3) {
            std::cout << "[PASSED] testGetSize\n";
        }
        else {
            std::cout << "[FAILED] testGetSize\n";
        }

        buffer.push("A");
        buffer.push("B");

        if (buffer.getCount() == 2) {
            std::cout << "[PASSED] testGetCount\n";
        }
        else {
            std::cout << "[FAILED] testGetCount\n";
        }
    }
}

//int main() {
//    std::cout << "Starting Circular Buffer Tests...\n";
//    runCircularBufferTests();
//    std::cout << "All tests completed.\n";
//    return 0;
//}
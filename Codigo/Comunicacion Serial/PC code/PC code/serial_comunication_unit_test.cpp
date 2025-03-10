#include "SerialComunication.h"
#include <chrono>
#include <iostream>

void runAllTests() {
    // Test: Set Available Ports
    {
        SerialComunication serial(10);
        if (serial.updateAvailablePorts()) {
            std::cout << "[PASSED] testSetAvailablePorts\n";
        }
        else {
            std::cout << "[FAILED] testSetAvailablePorts: No available ports found\n";
        }
    }

    // Test: Open and Close Port
    {
        SerialComunication serial(10);
        if (serial.updateAvailablePorts()) {
            auto ports = serial.getAvailablePorts();
            if (!ports.empty()) {
                if (serial.openPort(ports[0].first)) {
                    std::cout << "[PASSED] testOpenPort\n";
                    if (serial.closePort()) {
                        std::cout << "[PASSED] testClosePort\n";
                    }
                    else {
                        std::cout << "[FAILED] testClosePort\n";
                    }
                }
                else {
                    std::cout << "[FAILED] testOpenPort: Could not open COM port\n";
                }
            }
            else {
                std::cout << "[FAILED] testOpenPort: No ports available\n";
            }
        }
        else {
            std::cout << "[FAILED] testOpenPort: Could not update ports\n";
        }
    }

    // Test: Send and Receive
    {
        SerialComunication serial(10);
        if (serial.updateAvailablePorts()) {
            auto ports = serial.getAvailablePorts();
            if (!ports.empty()) {
                if (serial.openPort(ports[0].first)) {
                    serial.startReceiving();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait for serial to initialize

                    // First read might be empty, discard it
                    std::string firstMessage = serial.getReceivedMessage();
                    if (firstMessage.empty()) {
                        std::cout << "[INFO] Ignoring first empty message\n";
                        firstMessage = serial.getReceivedMessage(); // Try again
                    }

                    std::string message = "Hello, Serial!";

                    if (serial.send(message)) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Give time to receive
                        std::string receivedMessage;
                        if (serial.getReceivedMessage(receivedMessage)) {
                            if (receivedMessage == message) {
                                std::cout << "[PASSED] testSendAndReceive\n";
                            }
                            else {
                                std::cout << "[FAILED] testSendAndReceive: Expected '" << message << "', but got '" << receivedMessage << "'\n";
                            }
                        }
                        else {
                            std::cout << "[FAILED] testSendAndReceive: No message received\n";
                        }

                    }
                    else {
                        std::cout << "[FAILED] testSendAndReceive: Unable to send message\n";
                    }

                    serial.stopReceiving();
                    serial.closePort();
                }
                else {
                    std::cout << "[FAILED] testSendAndReceive: Unable to open port\n";
                }
            }
            else {
                std::cout << "[FAILED] testSendAndReceive: No available ports\n";
            }
        }
        else {
            std::cout << "[FAILED] testSendAndReceive: Unable to update ports\n";
        }
    }

    // Test: Continuous Write and Read
    {
        SerialComunication serial(10);
        if (serial.updateAvailablePorts()) {
            auto ports = serial.getAvailablePorts();
            if (!ports.empty()) {
                if (serial.openPort(ports[0].first)) {
                    serial.startReceiving();
                    std::string receivedMessage;
                    std::string message = "Continuous message";
                    bool success = true;

                    for (int i = 0; i < 5; ++i) { // Send 5 times
                        if (serial.send(message)) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait for response
                            serial.getReceivedMessage(receivedMessage);
                            std::cout << "[Test " << (i + 1) << "] Sent: " << message << " | Received: " << receivedMessage << std::endl;
                        }
                        else {
                            std::cout << "[FAILED] testContinuousWriteAndRead: Unable to send message\n";
                            success = false;
                            break;
                        }
                    }

                    if (success) {
                        std::cout << "[PASSED] testContinuousWriteAndRead\n";
                    }
                    serial.stopReceiving();
                    serial.closePort();
                }
                else {
                    std::cout << "[FAILED] testContinuousWriteAndRead: Unable to open port\n";
                }
            }
            else {
                std::cout << "[FAILED] testContinuousWriteAndRead: No available ports\n";
            }
        }
        else {
            std::cout << "[FAILED] testContinuousWriteAndRead: Unable to update ports\n";
        }
    }

    // Test: Invalid Port Handling
    {
        SerialComunication serial(10);
        int invalidPort = 9999;  // Non-existent port
        if (!serial.openPort(invalidPort)) {
            std::cout << "[PASSED] testInvalidPortHandling: Handled invalid port correctly\n";
        }
        else {
            std::cout << "[FAILED] testInvalidPortHandling: Opened an invalid port\n";
            serial.closePort();
        }
    }
}

//int main(int argc, char* argv[]) {
//    std::cout << "Starting Serial Communication Tests...\n";
//    runAllTests();
//    std::cout << "All tests completed.\n";
//    return 0;
//}
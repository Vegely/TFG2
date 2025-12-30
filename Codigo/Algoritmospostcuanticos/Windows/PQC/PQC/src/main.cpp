#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <memory>
#include <limits>
#include <thread> 
#include <chrono> 

#include "kyber.h"
#include "saber.h"
#include "hqc.h"

#include "SerialComunication.h"
#include "SerialPacketTransport.h"
#include "KEMProtocol.h"

#include "AlgorithmTester.h"


template <typename T>
std::vector<unsigned char> to_std_vector(const T& secure_vec) {
    return std::vector<unsigned char>(secure_vec.begin(), secure_vec.end());
}


template <typename VectorType>
void print_hex(const std::string& label, const VectorType& data) {
    std::cout << label << " (" << data.size() << " bytes): ";

    if (data.empty()) {
        std::cout << "(empty)" << std::endl;
        return;
    }

    for (size_t i = 0; i < std::min((size_t)20, data.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }

    if (data.size() > 20) std::cout << "...";
    std::cout << std::dec << std::endl;
}


void flush_serial_buffer(SerialComunication& serial) {
    std::string garbage;
    while (!serial.isBufferEmpty()) {
        serial.getReceivedMessage(garbage);
    }
    // Only print if we actually found something
    if (!garbage.empty()) std::cout << "[DEBUG] Flushed startup noise." << std::endl;
}


bool test_echo_connection(SerialComunication& serial, SerialPacketTransport& transport) {
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "[TEST] Starting Echo Check..." << std::endl;

    // Send 'E' to switch PSoC to Echo Mode
    serial.send("E");
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Safety delay

    // Create a test message
    std::string testMsg = "Hello PSoC!";
    std::vector<unsigned char> txData(testMsg.begin(), testMsg.end());

    std::cout << "[PC] Sending: \"" << testMsg << "\"" << std::endl;
    if (!transport.sendPacket(txData)) {
        std::cerr << "[FAIL] Failed to send echo packet." << std::endl;
        return false;
    }

    // Wait for reply
    std::vector<unsigned char> rxData;
    if (transport.receivePacket(rxData, 2000)) {
        std::string reply(rxData.begin(), rxData.end());
        std::cout << "[PSoC] Reply:   \"" << reply << "\"" << std::endl;

        if (rxData == txData) {
            std::cout << "[PASS] Echo Verification Successful." << std::endl;
            std::cout << "--------------------------------------\n" << std::endl;
            return true;
        }
        else {
            std::cerr << "[FAIL] Data mismatch!" << std::endl;
        }
    }
    else {
        std::cerr << "[FAIL] Timeout. PSoC did not reply." << std::endl;
    }
    std::cout << "--------------------------------------\n" << std::endl;
    return false;
}


void run_serial_session(int comPort, ProtocolRole role, std::unique_ptr<KeyExchange> kem_impl) {
    KEMProtocol kem(std::move(kem_impl), role);

    // Open Serial (Using Large Buffer 8192 to prevent overflow)
    SerialComunication serial(8192);
    if (!serial.openPort(comPort)) return;
    serial.startReceiving();

    std::cout << "[INFO] Waiting 2s for PSoC to boot..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    flush_serial_buffer(serial);

    SerialPacketTransport transport(serial);

    // Echo Test
    if (!test_echo_connection(serial, transport)) {
        std::cerr << "Aborting session due to connection failure.\n";
        return;
    }

    std::string modeCommand;
    // If PC is Initiator (Alice), PSoC must be Responder ('B')
    // If PC is Responder (Bob),   PSoC must be Initiator ('A')
    if (role == ProtocolRole::INITIATOR) {
        modeCommand = "B";
    }
    else {
        modeCommand = "A";
    }

    std::cout << "Configuring PSoC mode: " << modeCommand << std::endl;
    serial.send(modeCommand);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

 
    if (role == ProtocolRole::INITIATOR) {
        std::cout << "[Alice] Generating Public Key..." << std::endl;

        auto pk_secure = kem.start_negotiation();
        std::vector<unsigned char> pk = to_std_vector(pk_secure);

        print_hex("Generated PK", pk);

        std::cout << "[Alice] Sending PK (" << pk.size() << " bytes)..." << std::endl;
        if (!transport.sendPacket(pk)) return;

        std::cout << "[Alice] Waiting for Ciphertext..." << std::endl;
        std::vector<unsigned char> ct;

        if (transport.receivePacket(ct, 20000)) {
            print_hex("Received CT", ct);

            kem.process_message(ct);
        }
        else {
            std::cerr << "[Timeout] Did not receive Ciphertext from Bob." << std::endl;
            return;
        }
    }
    else {
        std::cout << "[Bob] Waiting for Public Key..." << std::endl;
        std::vector<unsigned char> pk;

        if (transport.receivePacket(pk, 20000)) {
            print_hex("Received PK", pk);

            std::cout << "[Bob] Encapsulating..." << std::endl;

            auto ct_secure = kem.process_message(pk);
            std::vector<unsigned char> ct = to_std_vector(ct_secure);

            print_hex("Generated CT", ct);

            std::cout << "[Bob] Sending Ciphertext..." << std::endl;
            transport.sendPacket(ct);
        }
        else {
            std::cerr << "[Timeout] Did not receive Public Key from Alice." << std::endl;
            return;
        }
    }

    if (kem.getState() == ProtocolState::ESTABLISHED) {
        auto secret_secure = kem.get_shared_secret();
        std::vector<unsigned char> secret_local = to_std_vector(secret_secure);

        std::cout << "\n[Local] Shared Secret Established!" << std::endl;
        print_hex("MY SECRET", secret_local);

        std::cout << "Waiting for PSoC to send its Secret for verification..." << std::endl;
        std::vector<unsigned char> secret_remote;

        if (transport.receivePacket(secret_remote, 25000)) {
            print_hex("PSOC SECRET", secret_remote);

            if (secret_local == secret_remote) {
                std::cout << "\n[SUCCESS] SECRETS MATCH! Handshake Verified." << std::endl;
            }
            else {
                std::cerr << "\n[FAILURE] CRITICAL: Secrets do NOT match!" << std::endl;
            }
        }
        else {
            std::cerr << "[FAIL] Timeout waiting for PSoC verification." << std::endl;
        }
    }
    else {
        std::cerr << "\n[FAILURE] Handshake failed." << std::endl;
    }

    serial.stopReceiving();
    serial.closePort();
}

int main(void) {
    // Comunications demo
    
    int port;
    int roleChoice;

    std::cout << "=== KEM Serial Link Manager ===" << std::endl;
    std::cout << "Available Ports:\n";
    {
        SerialComunication tempSerial(1);
        tempSerial.displayAvaileblePorts();
    }

    std::cout << "\nEnter COM port number (e.g. 6): ";
    std::cin >> port;

    std::cout << "\nSelect Role:\n";
    std::cout << "1. Initiator (Alice - Starts negotiation)\n";
    std::cout << "2. Responder (Bob - Waits for connection)\n";
    std::cout << "Choice: ";
    std::cin >> roleChoice;

    ProtocolRole role = (roleChoice == 1) ? ProtocolRole::INITIATOR : ProtocolRole::RESPONDER;

    run_serial_session(port, role, std::make_unique<Hqc>());

    std::cout << "\nSession ended. Press Enter to exit.";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    
    
    // Benchmarks for algorythms
    /*
    Saber hqc_impl;
    AlgorithmTester tester(&hqc_impl);

    std::cout << "Starting Benchmark for: " << hqc_impl.get_algorithm_name() << "\n\n";
    tester.runStackAnalysis();
    tester.runCycleAnalysis();
    tester.runThroughputAnalysis();

    std::cout << "Tests complete. Press Enter to exit...";
    std::cin.get();*/
    return 0;
}
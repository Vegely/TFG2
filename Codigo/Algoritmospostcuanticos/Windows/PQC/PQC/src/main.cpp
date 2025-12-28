#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <memory>
#include <limits>

#include "kyber.h"
#include "saber.h"
#include "hqc.h"

#include "SerialComunication.h"
#include "SerialPacketTransport.h"
#include "KEMProtocol.h"




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

template <typename AlgoType>
void run_test(const std::string& name) {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "TESTING ALGORITHM: " << name << std::endl;
    std::cout << "==========================================" << std::endl;

    try {
        // 1. Create two instances of the algorithm (one for Alice, one for Bob)
        std::unique_ptr<KeyExchange> alice_impl(new AlgoType());
        std::unique_ptr<KeyExchange> bob_impl(new AlgoType());

        // 2. Create the Protocol wrappers
        KEMProtocol alice(std::move(alice_impl), ProtocolRole::INITIATOR);
        KEMProtocol bob(std::move(bob_impl), ProtocolRole::RESPONDER);

        // 3. START: Alice generates Public Key
        std::cout << "[Alice] Generating Keys..." << std::endl;
        std::vector<unsigned char> pk = alice.start_negotiation();
        print_hex(" -> Public Key sent", pk);

        // 4. NETWORK: Send PK to Bob -> Bob generates Ciphertext
        std::cout << "[Bob]   Receiving PK..." << std::endl;
        std::vector<unsigned char> ct = bob.process_message(pk);
        print_hex(" <- Ciphertext sent", ct);

        // 5. NETWORK: Send CT to Alice -> Alice decapsulates
        std::cout << "[Alice] Receiving CT..." << std::endl;
        alice.process_message(ct);

        // 6. CHECK: Do they have the same secret?
        if (alice.getState() == ProtocolState::ESTABLISHED &&
            bob.getState() == ProtocolState::ESTABLISHED) {

            auto secret_a = alice.get_shared_secret();
            auto secret_b = bob.get_shared_secret();

            if (secret_a == secret_b) {
                std::cout << "[SUCCESS] Shared Secret Established!" << std::endl;
                print_hex("Shared Secret", secret_a);
            }
            else {
                std::cerr << "[FAILURE] Secrets do not match!" << std::endl;
            }
        }
        else {
            std::cerr << "[FAILURE] Protocol did not finish." << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
    }
}


void flush_serial_buffer(SerialComunication& serial) {
    std::string garbage;
    // Read repeatedly until the buffer is empty
    while (!serial.isBufferEmpty()) {
        serial.getReceivedMessage(garbage);
    }
    std::cout << "[DEBUG] Flushed startup noise." << std::endl;
}

bool test_echo_connection(SerialComunication& serial, SerialPacketTransport& transport) {
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "[TEST] Starting Echo Check..." << std::endl;

    // 1. Send the 'E' command to switch PSoC to Echo Mode
    serial.send("E");

    // Give PSoC a tiny moment to switch state
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 2. Create a test message
    std::string testMsg = "Hello PSoC! This is a connectivity test.";
    std::vector<unsigned char> txData(testMsg.begin(), testMsg.end());

    std::cout << "[PC] Sending: \"" << testMsg << "\"" << std::endl;
    transport.sendPacket(txData);

    // 3. Wait for reply
    std::vector<unsigned char> rxData;
    if (transport.receivePacket(rxData, 2000)) { // 2 second timeout

        // Convert back to string for display
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

void run_serial_session(int comPort, ProtocolRole role) {
    SerialComunication serial(4096);
    if (!serial.openPort(comPort)) return;
    serial.startReceiving();

    // Clear startup logs
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // (You can call your flush_serial_buffer(serial) here if you added it)

    SerialPacketTransport transport(serial);

    // ============================================================
    // STEP 1: RUN ECHO TEST
    // ============================================================
    // If this fails, we shouldn't proceed to crypto.
    if (!test_echo_connection(serial, transport)) {
        std::cerr << "Aborting session due to connection failure.\n";
        return;
    }

    // --- NEW: Send Mode Selection Byte ---
    std::string modeCommand;

    // If PC is Initiator, PSoC must be Responder ('B')
    // If PC is Responder, PSoC must be Initiator ('A')
    if (role == ProtocolRole::INITIATOR) {
        modeCommand = "B";
    }
    else {
        modeCommand = "A";
    }

    std::cout << "Configuring PSoC mode: " << modeCommand << std::endl;

    // Send raw command string (NOT a packet)
    serial.send(modeCommand);

    // Give PSoC a tiny moment to process the state switch
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // --- Continue with existing logic ---
    std::unique_ptr<KeyExchange> kem_impl(new Kyber());
    KEMProtocol kem(std::move(kem_impl), role);

    try {
        if (role == ProtocolRole::INITIATOR) {
            // ==========================================
            // ROLE: INITIATOR (Alice)
            // 1. Generate & Send Public Key
            // 2. Receive Ciphertext
            // 3. Decapsulate
            // ==========================================

            std::cout << "[Alice] Generating Public Key..." << std::endl;
            std::vector<unsigned char> pk = kem.start_negotiation();
            print_hex("Generated PK", pk);

            std::cout << "[Alice] Sending PK (" << pk.size() << " bytes)..." << std::endl;
            if (!transport.sendPacket(pk)) {
                std::cerr << "Error sending packet." << std::endl;
                return;
            }

            std::cout << "[Alice] Waiting for Ciphertext..." << std::endl;
            std::vector<unsigned char> ct;

            // Wait up to 10 seconds for Bob to reply
            if (transport.receivePacket(ct, 10000)) {
                print_hex("Received CT", ct);

                // Process CT (Decapsulate)
                kem.process_message(ct);
            }
            else {
                std::cerr << "[Timeout] Did not receive Ciphertext from Bob." << std::endl;
                return;
            }

        }
        else {
            // ==========================================
            // ROLE: RESPONDER (Bob)
            // 1. Receive Public Key
            // 2. Encapsulate & Generate Ciphertext
            // 3. Send Ciphertext
            // ==========================================

            std::cout << "[Bob] Waiting for Public Key..." << std::endl;
            std::vector<unsigned char> pk;

            // Wait up to 10 seconds for Alice to start
            if (transport.receivePacket(pk, 10000)) {
                print_hex("Received PK", pk);

                std::cout << "[Bob] Encapsulating..." << std::endl;
                // Process PK -> Generates CT inside KEMProtocol
                std::vector<unsigned char> ct = kem.process_message(pk);

                print_hex("Generated CT", ct);

                std::cout << "[Bob] Sending Ciphertext..." << std::endl;
                transport.sendPacket(ct);
            }
            else {
                std::cerr << "[Timeout] Did not receive Public Key from Alice." << std::endl;
                return;
            }
        }

        // 4. Verification
        if (kem.getState() == ProtocolState::ESTABLISHED) {
            auto secret = kem.get_shared_secret();
            std::cout << "\n[SUCCESS] Shared Secret Established!" << std::endl;
            print_hex("SHARED SECRET", secret);
        }
        else {
            std::cerr << "\n[FAILURE] Handshake failed." << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    // Cleanup
    serial.stopReceiving();
    serial.closePort();
}

int main(void) {
    int port;
    int roleChoice;

    std::cout << "=== KEM Serial Link Manager ===" << std::endl;

    // 1. Select Port
    std::cout << "Available Ports:\n";
    {
        // Use a temporary instance just to list ports
        SerialComunication tempSerial(1);
        tempSerial.displayAvaileblePorts();
    }

    std::cout << "\nEnter COM port number (e.g. 3 for COM3): ";
    std::cin >> port;

    // 2. Select Role
    std::cout << "\nSelect Role:\n";
    std::cout << "1. Initiator (Alice - Starts negotiation)\n";
    std::cout << "2. Responder (Bob - Waits for connection)\n";
    std::cout << "Choice: ";
    std::cin >> roleChoice;

    ProtocolRole role = (roleChoice == 1) ? ProtocolRole::INITIATOR : ProtocolRole::RESPONDER;

    // 3. Start
    run_serial_session(port, role);

    std::cout << "\nSession ended. Press Enter to exit.";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}
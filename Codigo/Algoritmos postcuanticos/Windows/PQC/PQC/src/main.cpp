#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <memory>

// Include your interface headers
#include "KEMProtocol.h"
#include "kyber.h"
#include "saber.h"
#include "hqc.h"

// ---------------------------------------------------------
// Helper: Print bytes in Hex
// ---------------------------------------------------------
template <typename VectorType>
void print_hex(const std::string& label, const VectorType& data) {
    std::cout << label << " (" << data.size() << " bytes): ";

    // Safety check to avoid crashing on empty vectors
    if (data.empty()) {
        std::cout << "(empty)" << std::endl;
        return;
    }

    // Loop through the first 8 bytes (or fewer)
    for (size_t i = 0; i < std::min((size_t)20, data.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }

    if (data.size() > 20) std::cout << "...";
    std::cout << std::dec << std::endl;
}
// ---------------------------------------------------------
// Helper: Run Simulation for One Algorithm
// ---------------------------------------------------------
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
int main(void) {
    run_test<Kyber>("Kyber-1024");
    run_test<Saber>("Saber-Fire");
    run_test<Hqc>("HQC-256");

    return 0;
}
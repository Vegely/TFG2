#ifndef KEM_PROTOCOL_H
#define KEM_PROTOCOL_H

#include "keyexhange.h"
#include "secureVector.h"
#include <vector>
#include <memory>


enum class ProtocolRole {
    INITIATOR, // Alice: Generates PK, sends PK, receives CT, Decapsulates
    RESPONDER  // Bob: Receives PK, Encapsulates, sends CT
};

enum class ProtocolState {
    INIT,           
    WAITING_FOR_CT, // Initiator generated PK, waiting for response
    WAITING_FOR_PK, // Responder waiting for start message
    ESTABLISHED,    // Key exchange complete, Shared Secret is valid
    ERROR           
};

class KEMProtocol {
public:

    KEMProtocol(std::unique_ptr<KeyExchange> kem_impl, ProtocolRole role);
    ProtocolState getState() const;


    // Initiator only: generates a keypair and returns the pk to be sent, returns the pk bytes.
    std::vector<unsigned char> start_negotiation();


    // Process Incoming data and sends back the response bytes
    std::vector<unsigned char> process_message(const std::vector<unsigned char>& input_msg);


    // Establishes the shared secret and returns it
    const Secure::SecureVector& get_shared_secret() const;

private:
    std::unique_ptr<KeyExchange> kem_;
    ProtocolRole role_;
    ProtocolState state_;

    // Secure Buffers using your SecureVector implementation
    Secure::SecureVector secret_key_;
    Secure::SecureVector shared_secret_;

    // Private helpers
    std::vector<unsigned char> handle_encapsulation(const std::vector<unsigned char>& pk);
    void handle_decapsulation(const std::vector<unsigned char>& ct);
    void transition_to_error(const char* msg);
};

#endif // KEM_PROTOCOL_H
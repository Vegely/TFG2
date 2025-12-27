#include "KEMProtocol.h"
#include <stdexcept>
#include <iostream>

KEMProtocol::KEMProtocol(std::unique_ptr<KeyExchange> kem_impl, ProtocolRole role): kem_(std::move(kem_impl)), role_(role), state_(ProtocolState::INIT)
{
    if (!kem_) {
        throw std::invalid_argument("KEM implementation cannot be null.");
    }
    shared_secret_.resize(kem_->get_shared_secret_size());
}

ProtocolState KEMProtocol::getState() const {
    return state_;
}

std::vector<unsigned char> KEMProtocol::start_negotiation() {
    if (role_ != ProtocolRole::INITIATOR) {
        transition_to_error("Only INITIATOR can start negotiation.");
        return {};
    }

    try {
        std::vector<unsigned char> public_key(kem_->get_public_key_size());
        secret_key_.resize(kem_->get_secret_key_size());

        if (kem_->keypair(public_key.data(), secret_key_.data()) != 0) {
            transition_to_error("Keypair generation failed.");
            return {};
        }

        state_ = ProtocolState::WAITING_FOR_CT;
        return public_key;
    }
    catch (const std::exception& e) {
        transition_to_error(e.what());
        return {};
    }
}

std::vector<unsigned char> KEMProtocol::process_message(const std::vector<unsigned char>& input_msg) {
    std::vector<unsigned char> response_data;

    try {
        switch (state_) {
        case ProtocolState::INIT:
            if (role_ == ProtocolRole::RESPONDER) {
                response_data = handle_encapsulation(input_msg);
            }
            else {
                transition_to_error("Initiator received data in INIT state (expected to start).");
            }
            break;

        case ProtocolState::WAITING_FOR_CT:
            // Initiator receives Ciphertext from Responder
            if (role_ == ProtocolRole::INITIATOR) {
                handle_decapsulation(input_msg);
            }
            else {
                transition_to_error("Responder logic error: reached WAITING_FOR_CT.");
            }
            break;

        case ProtocolState::ESTABLISHED:
            break;

        case ProtocolState::ERROR:
        default:
            break;
        }
    }
    catch (const std::exception& e) {
        transition_to_error(e.what());
    }

    return response_data;
}

const Secure::SecureVector& KEMProtocol::get_shared_secret() const {
    if (state_ != ProtocolState::ESTABLISHED) {
        throw std::runtime_error("Key not established yet.");
    }
    return shared_secret_;
}


std::vector<unsigned char> KEMProtocol::handle_encapsulation(const std::vector<unsigned char>& pk) {
    if (pk.size() != kem_->get_public_key_size()) {
        transition_to_error("Received PK size mismatch.");
        return {};
    }

    std::vector<unsigned char> ciphertext(kem_->get_ciphertext_size());
    if (kem_->encapsulate(ciphertext.data(), shared_secret_.data(), pk.data()) != 0) {
        transition_to_error("Encapsulation failed.");
        return {};
    }

    state_ = ProtocolState::ESTABLISHED; 
    return ciphertext;
}

void KEMProtocol::handle_decapsulation(const std::vector<unsigned char>& ct) {
    if (ct.size() != kem_->get_ciphertext_size()) {
        transition_to_error("Received Ciphertext size mismatch.");
        return;
    }

    if (kem_->decapsulate(shared_secret_.data(), ct.data(), secret_key_.data()) != 0) {
        transition_to_error("Decapsulation failed.");
        return;
    }

    // Wipe secret key after use (SecureVector handles memory zeroing on clear/destruct)
    secret_key_.clear();

    state_ = ProtocolState::ESTABLISHED; 
}

void KEMProtocol::transition_to_error(const char* msg) {
    state_ = ProtocolState::ERROR;
    secret_key_.clear();
    shared_secret_.clear();
    std::cerr << "[KEM-Proto Error] " << msg << std::endl;
   
}
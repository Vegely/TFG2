#ifndef SABER_H
#define SABER_H

#include "keyexhange.h"
#include "saber_wrapper.h"
#include <stdexcept>

class Saber :public KeyExchange
{
public:
    size_t get_public_key_size()    const override { return saber_get_public_key_size(); }
    size_t get_secret_key_size()    const override { return saber_get_secret_key_size(); }
    size_t get_ciphertext_size()    const override { return saber_get_ciphertext_size(); }
    size_t get_shared_secret_size() const override { return saber_get_shared_secret_size(); }

    Saber() {
        static const bool ready = []() {
            if (saber_init(SABER_SECURITY_FIRE) != 0) {
                throw std::runtime_error("Saber Init failed.");
            }
            return true;
            }();

        (void)ready;
    }

    const char* get_algorithm_name() const override { return "KYBER_1024"; }

    int keypair(unsigned char* pk, unsigned char* sk) override {
        return saber_keypair(pk, sk);
    }

    int encapsulate(unsigned char* ct, unsigned char* ss, const unsigned char* pk) override {
        return saber_encapsulate(ct, ss, pk);
    }

    int decapsulate(unsigned char* ss, const unsigned char* ct, const unsigned char* sk) override {
        return saber_decapsulate(ss, ct, sk);
    }
};

#endif // SABER_H
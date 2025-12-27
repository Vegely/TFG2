#ifndef KYBER_H
#define KYBER_H

#include "keyexhange.h"
#include "kyber_wrapper.h"
#include <stdexcept>

class Kyber :public KeyExchange
{
public:
    size_t get_public_key_size()    const override { return kyber_get_public_key_size();}
    size_t get_secret_key_size()    const override { return kyber_get_secret_key_size();}
    size_t get_ciphertext_size()    const override { return kyber_get_ciphertext_size();}
    size_t get_shared_secret_size() const override { return kyber_get_shared_secret_size();}

    Kyber() {
        static const bool ready = []() {
            int res = kyber_init(KYBER_SECURITY_1024);

            if (res != 0) {
                throw std::runtime_error("KYBER Init failed: Library not compiled for HQC-256");
            }
            return true;
            }();

        (void)ready;
    }

    const char* get_algorithm_name() const override { return "KYBER_1024"; }

    int keypair(unsigned char* pk, unsigned char* sk) override {
        return kyber_keypair(pk, sk);
    }

    int encapsulate(unsigned char* ct, unsigned char* ss, const unsigned char* pk) override {
        return kyber_encapsulate(ct, ss, pk);
    }

    int decapsulate(unsigned char* ss, const unsigned char* ct, const unsigned char* sk) override {
        return kyber_decapsulate(ss, ct, sk);
    }
};


#endif // KYBER_H
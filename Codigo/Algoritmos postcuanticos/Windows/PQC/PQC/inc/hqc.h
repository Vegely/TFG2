#ifndef HQC_H
#define HQC_H

#include "keyexhange.h"
#include "hqc_wrapper.h"
#include <stdexcept>

class Hqc:public KeyExchange
{
public:
    size_t get_public_key_size()    const override { return hqc_get_public_key_size(); }
    size_t get_secret_key_size()    const override { return hqc_get_secret_key_size(); }
    size_t get_ciphertext_size()    const override { return hqc_get_ciphertext_size(); }
    size_t get_shared_secret_size() const override { return hqc_get_shared_secret_size(); }

    Hqc() {
        static const bool ready = []() {
            int res = hqc_init(HQC_SECURITY_256);

            if (res != 0) {
                throw std::runtime_error("HQC Init failed: Library not compiled for HQC-256");
            }
            return true;
            }();

        (void)ready;
    }

    const char* get_algorithm_name() const override { return "KYBER_1024"; }

    int keypair(unsigned char* pk, unsigned char* sk) override {
        return hqc_keypair(pk, sk);
    }

    int encapsulate(unsigned char* ct, unsigned char* ss, const unsigned char* pk) override {
        return hqc_encapsulate(ct, ss, pk);
    }

    int decapsulate(unsigned char* ss, const unsigned char* ct, const unsigned char* sk) override {
        return hqc_decapsulate(ss, ct, sk);
    }
};

#endif // HQC_H
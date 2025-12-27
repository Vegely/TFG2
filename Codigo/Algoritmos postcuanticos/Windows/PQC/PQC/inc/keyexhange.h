#ifndef KEYEXCHANGE_H
#define KEYEXCHANGE_H

class KeyExchange
{
public:

    virtual ~KeyExchange() = default;
    virtual size_t get_public_key_size() const = 0;
    virtual size_t get_secret_key_size() const = 0;
    virtual size_t get_ciphertext_size() const = 0;
    virtual size_t get_shared_secret_size() const = 0;
    virtual const char* get_algorithm_name() const = 0;
    virtual int keypair(unsigned char* pk, unsigned char* sk) = 0;
    virtual int encapsulate(unsigned char* ct, unsigned char* ss, const unsigned char* pk) = 0;
    virtual int decapsulate(unsigned char* ss, const unsigned char* ct, const unsigned char* sk) = 0;
};


#endif // KEYEXCHANGE_H
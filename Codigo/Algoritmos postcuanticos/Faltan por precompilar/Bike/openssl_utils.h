/******************************************************************************
 * BIKE -- Bit Flipping Key Encapsulation
 *
 * Copyright (c) 2021 Nir Drucker, Shay Gueron, Rafael Misoczki, Tobias Oder,
 * Tim Gueneysu, Jan Richter-Brockmann.
 * Contact: drucker.nir@gmail.com, shay.gueron@gmail.com,
 * rafaelmisoczki@google.com, tobias.oder@rub.de, tim.gueneysu@rub.de,
 * jan.richter-brockmann@rub.de.
 *
 * Permission to use this code BIKE is granted.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * The names of the contributors may not be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ""AS IS"" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS CORPORATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef _OSSL_UTILITIES_H_
#define _OSSL_UTILITIES_H_

#include "string.h"
#include "types.h"
#include "utilities.h"
#include "openssl/bn.h"

//Perform a cyclic product by using OpenSSL.
_INLINE_ status_t cyclic_product(OUT BIGNUM *r,
        IN const BIGNUM *a,
        IN const BIGNUM *b)
{
    BN_CTX *bn_ctx = BN_CTX_new();
    BIGNUM *m = BN_new();
    status_t res = SUCCESS;

    // m = x^PARAM_R - 1
    if((BN_set_bit(m, R_BITS) == 0) ||
            (BN_set_bit(m, 0) == 0))
    {
        ERR(E_OSSL_FAILURE);
    }

    // r = a*b mod m
    if(BN_GF2m_mod_mul(r, a, b, m, bn_ctx) == 0)
    {
        ERR(E_OSSL_FAILURE);
    }

    EXIT:
    //Cleanup
    BN_free(m);
    BN_CTX_free(bn_ctx);

    return res;
}

_INLINE_ status_t invert_poly(OUT BIGNUM *r,
        IN const BIGNUM *a)
{
    BN_CTX *bn_ctx = BN_CTX_new();
    BIGNUM *m = BN_new();
    status_t res = SUCCESS;

    // m = x^PARAM_R - 1
    if((BN_set_bit(m, R_BITS) == 0) ||
            (BN_set_bit(m, 0) == 0))
    {
        ERR(E_OSSL_FAILURE);
    }

    // r = a*b mod m
    if(BN_GF2m_mod_inv (r, a, m, bn_ctx) == 0)
    {
        ERR(E_OSSL_FAILURE);
    }

    EXIT:
    //Cleanup
    BN_free(m);
    BN_CTX_free(bn_ctx);

    return res;
}

//Loading numbers into OpenSSL should be done in Big Endian representation.
//Therefore the byte order of a number should be reversed.
_INLINE_ void reverse_endian(OUT uint8_t *res,
        IN const uint8_t *in,
        IN const uint32_t n)
{
    uint32_t i;
    uint64_t tmp;

    for(i = 0 ; i < (n/2) ; i++)
    {
        tmp = in[i];
        res[i] = in[n-1-i];
        res[n-1-i] = tmp;
    }

    //If the number of blocks is odd swap also the middle block.
    if (n%2)
    {
        res[i] = in[i];
    }
}

_INLINE_ status_t ossl_bn2bin(OUT uint8_t* out,
        IN const BIGNUM* in,
        IN const uint32_t size)
{
    uint8_t be_tmp[size];
    memset(out, 0, size);

    if (BN_bn2bin(in, be_tmp) == 0)
    {
        return E_OSSL_FAILURE;
    }
    reverse_endian(out, be_tmp, BN_num_bytes(in));

    return SUCCESS;
}

_INLINE_ status_t ossl_bin2bn(IN BIGNUM* out,
        OUT const uint8_t* in,
        IN const uint32_t size)
{
    uint8_t be_tmp[size];
    memset(be_tmp, 0, size);

    reverse_endian(be_tmp, in, size);

    if(BN_bin2bn(be_tmp, size, out) == 0)
    {
        return E_OSSL_FAILURE;
    }

    return SUCCESS;
}

_INLINE_ status_t print_ossl_bn(IN const BIGNUM* bn, IN const uint64_t size)
{
    uint8_t tmp[size];
    if(ossl_bn2bin(tmp, bn, size))
    {
        return E_OSSL_FAILURE;
    }

    print((uint64_t*)tmp, size*8);

    return SUCCESS;
}

#endif //_OSSL_UTILITIES_H_

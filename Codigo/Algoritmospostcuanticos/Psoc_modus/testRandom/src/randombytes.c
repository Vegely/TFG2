#include "randombytes.h"

/* MODUSTOOLBOX INCLUDES */
#include "cy_pdl.h"
#include "cy_crypto_core.h"

/* Standard PSoC 6 TRNG Polynomials */
#define GARO_POLY       (0x1D291250UL)
#define FIRO_POLY       (0x17592384UL)

/* * Helper to ensure Crypto block is on. 
 * We check if it's already enabled to avoid resetting it if main() did it.
 */
static void crypto_ensure_init(void)
{
    if (!Cy_Crypto_Core_IsEnabled(CRYPTO))
    {
        Cy_Crypto_Core_Enable(CRYPTO);
    }
}

void randombytes(uint8_t *out, size_t outlen)
{
    uint32_t random_chunk;
    size_t i;

    /* 1. Make sure hardware is on */
    crypto_ensure_init();

    /* 2. Loop until we have filled the buffer */
    while (outlen > 0)
    {
        /* Generate 32 bits (4 bytes) of random data at once */
        Cy_Crypto_Core_Trng(CRYPTO, GARO_POLY, FIRO_POLY, 32, &random_chunk);

        /* Copy up to 4 bytes from the random chunk to the output */
        /* We handle the edge case where outlen < 4 */
        for (i = 0; i < 4 && outlen > 0; i++)
        {
            *out = (uint8_t)(random_chunk & 0xFF);
            random_chunk >>= 8; /* Shift to get the next byte */
            out++;
            outlen--;
        }
    }
}
#include "project.h"
#include "cy_crypto.h"
#include "cy_crypto_server.h"
#include "randombytes.h"

/* ===== Local crypto contexts ===== */
static cy_stc_crypto_context_t cryptoScratch;
static cy_stc_crypto_server_context_t cryptoServerContext;
static cy_stc_crypto_context_trng_t trngContext;

/* ===== TRNG configuration (same as CE221295) ===== */
#define MY_CHAN_CRYPTO          (uint32_t)(3u)
#define MY_INTR_CRYPTO_SRV      (uint32_t)(1u)
#define MY_INTR_CRYPTO_CLI      (uint32_t)(2u)
#define MY_INTR_CRYPTO_SRV_MUX  (IRQn_Type)(2u)
#define MY_INTR_CRYPTO_CLI_MUX  (IRQn_Type)(3u)
#define MY_INTR_CRYPTO_ERR_MUX  (IRQn_Type)(4u)

#define CRYPTO_TRNG_GARO_POL    (0x42000000)
#define CRYPTO_TRNG_FIRO_POL    (0x43000000)

/* Crypto hardware configuration */
static const cy_stc_crypto_config_t cryptoConfig =
{
    .ipcChannel             = MY_CHAN_CRYPTO,
    .acquireNotifierChannel = MY_INTR_CRYPTO_SRV,
    .releaseNotifierChannel = MY_INTR_CRYPTO_CLI,
    .releaseNotifierConfig  = {
        .intrSrc      = cpuss_interrupts_ipc_2_IRQn,
        .intrPriority = 2u,
    },
    .userCompleteCallback   = NULL,
    .userGetDataHandler     = NULL,
    .userErrorHandler       = NULL,
    .acquireNotifierConfig  = {
        .intrSrc      = cpuss_interrupts_ipc_1_IRQn,
        .intrPriority = 2u,
    },
    .cryptoErrorIntrConfig  = {
        .intrSrc      = cpuss_interrupt_crypto_IRQn,
        .intrPriority = 2u,
    },
};

/* ===== Initialization helper ===== */
static void crypto_init(void)
{
    static bool initialized = false;
    if (initialized)
        return;

    __enable_irq();

    /* Start and enable Crypto hardware */
    Cy_Crypto_Server_Start(&cryptoConfig, &cryptoServerContext);
    Cy_Crypto_Init(&cryptoConfig, &cryptoScratch);
    Cy_Crypto_Enable();
    Cy_Crypto_Sync(CY_CRYPTO_SYNC_BLOCKING);

    initialized = true;
}

/* ===== Implementation of randombytes() ===== */
void randombytes(uint8_t *out, size_t outlen)
{
    uint32_t rnd;
    crypto_init();

    for (size_t i = 0; i < outlen; i++)
    {
        /* Generate 8 bits (0–255) of random data */
        Cy_Crypto_Trng_Generate(
            CRYPTO_TRNG_GARO_POL,
            CRYPTO_TRNG_FIRO_POL,
            7u,          /* bit size: 0–7 => 8 bits */
            &rnd,
            &trngContext
        );

        Cy_Crypto_Sync(CY_CRYPTO_SYNC_BLOCKING);
        out[i] = (uint8_t)(rnd & 0xFF);
    }
}

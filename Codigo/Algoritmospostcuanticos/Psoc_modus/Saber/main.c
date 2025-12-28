/******************************************************************************
* File Name:   main.c
* Description: Saber Crypto Example for ModusToolbox (PSoC 6)
******************************************************************************/

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cy_crypto_core.h" /* REQUIRED for Hardware Random Number Generator */

#include <stdio.h>
#include <stdlib.h>

/* * NOTE: Ensure you added "INCLUDES+= ./inc" to your Makefile so this works.
 * If not, use: #include "../inc/saber_wrapper.h"
 */
#include "saber_wrapper.h"

/*******************************************************************************
* Function Name: main
********************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* 1. Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* 2. Enable global interrupts */
    __enable_irq();

    /* 3. Initialize UART (Retarget I/O) for printf functionality 
     * This handles buffering automatically (replacing your old UART_Isr)
     */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* 4. IMPORTANT: Enable the PSoC 6 Crypto Hardware 
     * If you skip this, the TRNG will fail and the chip will crash.
     */
    Cy_Crypto_Core_Enable(CRYPTO);

    /* Turn off stdout buffering so prints appear immediately */
    setvbuf(stdout, NULL, _IONBF, 0);

    /* Print Banner */
    printf("\x1b[2J\x1b[;H"); // Clear Screen
    printf("================================\r\n");
    printf(" System Start (ModusToolbox - Saber)\r\n");
    printf("================================\r\n");

    /* 5. Initialize Saber Logic */
    /* Note: Ensure SABER_SECURITY_FIRE is defined in your header */
    if (saber_init(SABER_SECURITY_FIRE) != 0) {
        printf("Saber init failed\r\n");
    }

    // Get Sizes
    const size_t BYTES_SK = saber_get_secret_key_size();
    const size_t BYTES_PK = saber_get_public_key_size();
    const size_t BYTES_CT = saber_get_ciphertext_size();
    const size_t BYTES_SS = saber_get_shared_secret_size();

    printf("Saber Sizes:\r\n");
    printf("  Public Key Size   : %lu bytes\r\n", (unsigned long)BYTES_PK);
    printf("  Secret Key Size   : %lu bytes\r\n", (unsigned long)BYTES_SK);
    printf("  Ciphertext Size   : %lu bytes\r\n", (unsigned long)BYTES_CT);
    printf("  Shared Secret Size: %lu bytes\r\n\r\n", (unsigned long)BYTES_SS);

    /* 6. Allocate Memory */
    /* Check Makefile HEAP_SIZE if this fails (0x8000 is recommended) */
    unsigned char *sk = (unsigned char *) calloc(BYTES_SK, sizeof(unsigned char));
    unsigned char *pk = (unsigned char *) calloc(BYTES_PK, sizeof(unsigned char));
    unsigned char *ct = (unsigned char *) calloc(BYTES_CT, sizeof(unsigned char));
    unsigned char *ss = (unsigned char *) calloc(BYTES_SS, sizeof(unsigned char));

    if (!sk || !pk || !ct || !ss) {
        printf("CRITICAL: Memory allocation failed.\r\n");
        printf("SOLUTION: Increase HEAP_SIZE in your Makefile.\r\n");
    } else {
        printf("Memory allocated successfully.\r\n");

        /* Key Generation */
        printf("Generating Keys... ");
        if (saber_keypair(pk, sk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");

        /* Encapsulation */
        printf("Encapsulating... ");
        if (saber_encapsulate(ct, ss, pk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");

        /* Decapsulation */
        printf("Decapsulating... ");
        if (saber_decapsulate(ss, ct, sk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");
        
        printf("\r\n--- Saber Crypto Test Complete ---\r\n");
    }

    /* Cleanup */
    free(sk); free(pk); free(ct); free(ss);

    printf("Entering Command Loop.\r\n");

    for(;;)
    {
        /* ModusToolbox handles UART receiving in the background. */
        cyhal_system_delay_ms(100); 
    }
}
/* [] END OF FILE */
/******************************************************************************
* File Name:   main.c
* Description: Kyber Crypto Example for ModusToolbox (PSoC 6)
******************************************************************************/

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cy_crypto_core.h" /* REQUIRED for Hardware Random Number Generator */

#include <stdio.h>
#include <stdlib.h>

/* * Since we added "-I./inc" to the Makefile, we can include this directly.
 * If you didn't update the Makefile, use "../inc/kyber_wrapper.h"
 */
#include "kyber_wrapper.h"

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
     * If you skip this, randombytes() will crash! 
     */
    Cy_Crypto_Core_Enable(CRYPTO);

    /* Turn off stdout buffering so prints appear immediately */
    setvbuf(stdout, NULL, _IONBF, 0);

    /* Print Banner */
    printf("\x1b[2J\x1b[;H"); // Clear Screen
    printf("================================\r\n");
    printf(" System Start (ModusToolbox)\r\n");
    printf("================================\r\n");

    /* 5. Initialize Kyber Logic */
    // Ensure you pick the level matching your header files (e.g., KYBER_SECURITY_1024 or 768)
    if (kyber_init(KYBER_SECURITY_1024) != 0) {
        printf("Kyber init failed\r\n");
    }

    // Get Sizes
    const size_t BYTES_SK = kyber_get_secret_key_size();
    const size_t BYTES_PK = kyber_get_public_key_size();
    const size_t BYTES_CT = kyber_get_ciphertext_size();
    const size_t BYTES_SS = kyber_get_shared_secret_size();

    printf("Kyber Sizes:\r\n");
    printf("  Public Key Size   : %lu bytes\r\n", (unsigned long)BYTES_PK);
    printf("  Secret Key Size   : %lu bytes\r\n", (unsigned long)BYTES_SK);
    printf("  Ciphertext Size   : %lu bytes\r\n", (unsigned long)BYTES_CT);
    printf("  Shared Secret Size: %lu bytes\r\n\r\n", (unsigned long)BYTES_SS);

    /* 6. Allocate Memory */
    /* Note: If this fails, you must increase "Heap Size" in the BSP Makefile or linker script */
    unsigned char *sk = (unsigned char *) calloc(BYTES_SK, sizeof(unsigned char));
    unsigned char *pk = (unsigned char *) calloc(BYTES_PK, sizeof(unsigned char));
    unsigned char *ct = (unsigned char *) calloc(BYTES_CT, sizeof(unsigned char));
    unsigned char *ss = (unsigned char *) calloc(BYTES_SS, sizeof(unsigned char));

    if (!sk || !pk || !ct || !ss) {
        printf("CRITICAL: Memory allocation failed.\r\n");
        printf("Solution: Open 'Makefile', find 'HEAP_SIZE', and increase it (try 0x4000).\r\n");
    } else {
        printf("Memory allocated successfully.\r\n");

        /* Key Generation */
        printf("Generating Keys... ");
        if (kyber_keypair(pk, sk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");

        /* Encapsulation */
        printf("Encapsulating... ");
        if (kyber_encapsulate(ct, ss, pk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");

        /* Decapsulation */
        printf("Decapsulating... ");
        if (kyber_decapsulate(ss, ct, sk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");
        
        printf("\r\n--- Crypto Test Complete ---\r\n");
    }

    /* Cleanup */
    free(sk); free(pk); free(ct); free(ss);

    printf("Entering Command Loop.\r\n");

    for(;;)
    {
        /* ModusToolbox handles UART receiving in the background.
           You can use 'getchar()' here if you need input later. */
        cyhal_system_delay_ms(100); 
    }
}
/* [] END OF FILE */
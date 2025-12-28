/******************************************************************************
* File Name:   main.c
* Description: HQC Crypto Example for ModusToolbox (PSoC 6)
******************************************************************************/

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cy_crypto_core.h" /* REQUIRED for Hardware Random Number Generator */

#include <stdio.h>
#include <stdlib.h>

/* * If you added "INCLUDES+= ./inc" to your Makefile, use this:
 */
#include "hqc_wrapper.h"

/* * If you did NOT update your Makefile, you might need:
 * #include "../inc/hqc_wrapper.h" 
 */

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
     * This replaces your old "UART_Start" and ISR code.
     */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* 4. IMPORTANT: Enable the PSoC 6 Crypto Hardware 
     * If you skip this, the TRNG will not work and code will crash.
     */
    Cy_Crypto_Core_Enable(CRYPTO);

    /* Turn off stdout buffering so prints appear immediately */
    setvbuf(stdout, NULL, _IONBF, 0);

    /* Print Banner */
    printf("\x1b[2J\x1b[;H"); // Clear Screen
    printf("================================\r\n");
    printf(" System Start (ModusToolbox - HQC)\r\n");
    printf("================================\r\n");

    /* 5. Initialize HQC Logic */
    // Using HQC-256 (High Security) as requested
    if (hqc_init(HQC_SECURITY_256) != 0) {
        printf("HQC init failed\r\n");
    }

    // Get Sizes
    const size_t BYTES_SK = hqc_get_secret_key_size();
    const size_t BYTES_PK = hqc_get_public_key_size();
    const size_t BYTES_CT = hqc_get_ciphertext_size();
    const size_t BYTES_SS = hqc_get_shared_secret_size();

    printf("HQC Sizes:\r\n");
    printf("  Public Key Size   : %lu bytes\r\n", (unsigned long)BYTES_PK);
    printf("  Secret Key Size   : %lu bytes\r\n", (unsigned long)BYTES_SK);
    printf("  Ciphertext Size   : %lu bytes\r\n", (unsigned long)BYTES_CT);
    printf("  Shared Secret Size: %lu bytes\r\n\r\n", (unsigned long)BYTES_SS);

    /* 6. Allocate Memory */
    /* WARNING: HQC requires a large HEAP. If this fails, edit Makefile HEAP_SIZE */
    unsigned char *sk = (unsigned char *) calloc(BYTES_SK, sizeof(unsigned char));
    unsigned char *pk = (unsigned char *) calloc(BYTES_PK, sizeof(unsigned char));
    unsigned char *ct = (unsigned char *) calloc(BYTES_CT, sizeof(unsigned char));
    unsigned char *ss = (unsigned char *) calloc(BYTES_SS, sizeof(unsigned char));

    if (!sk || !pk || !ct || !ss) {
        printf("CRITICAL: Memory allocation failed.\r\n");
        printf("SOLUTION: Open 'Makefile', find 'HEAP_SIZE', and increase it to 0xC000 (48KB).\r\n");
    } else {
        printf("Memory allocated successfully.\r\n");

        /* Key Generation */
        printf("Generating Keys... ");
        if (hqc_keypair(pk, sk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");

        /* Encapsulation */
        printf("Encapsulating... ");
        if (hqc_encapsulate(ct, ss, pk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");

        /* Decapsulation */
        printf("Decapsulating... ");
        if (hqc_decapsulate(ss, ct, sk) != 0) printf("FAILED\r\n");
        else printf("SUCCESS\r\n");
        
        printf("\r\n--- HQC Crypto Test Complete ---\r\n");
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
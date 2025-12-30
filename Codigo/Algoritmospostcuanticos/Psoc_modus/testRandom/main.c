#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <stdio.h>

/* Include your header so we can call the function */
#include "randombytes.h" 

/* * BAUD RATE CONFIGURATION
 * KitProg3 (the chip on your board) generally supports up to 4,000,000.
 * At 4Mbps, 1GB takes ~45 minutes.
 * If you see data corruption, lower this to 921600 (takes ~3 hours).
 */
#define UART_BAUD_RATE 4000000

/* * Transfer Block Size
 * 4KB is an optimal chunk size for USB-UART bridges.
 */
#define CHUNK_SIZE 4096 
uint8_t buffer[CHUNK_SIZE];

int main(void)
{
    cy_rslt_t result;

    /* 1. Initialize Board Support Package */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) { CY_ASSERT(0); }

    /* 2. Enable Interrupts */
    __enable_irq();

    /* 3. Initialize UART for printf/fwrite at High Speed */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, UART_BAUD_RATE);
    if (result != CY_RSLT_SUCCESS) { CY_ASSERT(0); }

    /* * 4. CRITICAL: Disable StdLib Buffering 
     * This ensures 'fwrite' sends data immediately without trying to format it.
     */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    /* * 5. Handshake
     * Wait for the character 's' from the PC before starting.
     * This prevents filling the PC buffer with junk while you are setting up.
     */
    while(getchar() != 's');

    /* 6. Main Data Loop */
    for(;;)
    {
        /* Call your unmodified function to fill the buffer */
        randombytes(buffer, CHUNK_SIZE);

        /* Write the raw binary buffer to UART */
        /* fwrite is used because printf handles null terminators \0 incorrectly */
        fwrite(buffer, 1, CHUNK_SIZE, stdout);
    }
}
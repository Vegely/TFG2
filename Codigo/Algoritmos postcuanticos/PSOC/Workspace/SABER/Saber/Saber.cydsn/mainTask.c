/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <stdlib.h>
#include "../saber_wrapper.h"

unsigned char *sk = NULL;
unsigned char *pk = NULL;
unsigned char *ct = NULL;
unsigned char *ss = NULL;

// External semaphore from UART task
extern SemaphoreHandle_t uartReadySemaphore;

void mainTask(void *arg)
{
    (void)arg;
    
    // Wait for UART to be ready (block until semaphore is given)
    xSemaphoreTake(uartReadySemaphore, portMAX_DELAY);
    
    printf("Main task started - UART is ready\r\n");
    
    //Initialize SABER
    if (saber_init(SABER_SECURITY_FIRE) != 0) {
        printf("FireSaber init failed\n");
    }
    
    // Allocate memory
    const size_t BYTES_SK = saber_get_secret_key_size();
    const size_t BYTES_PK = saber_get_public_key_size();
    const size_t BYTES_CT = saber_get_ciphertext_size();
    const size_t BYTES_SS = saber_get_shared_secret_size();

    printf("FireSaber Sizes:\r\n");
    printf("  Public Key Size     : %lu bytes\r\n", (unsigned long)BYTES_PK);
    printf("  Secret Key Size     : %lu bytes\r\n", (unsigned long)BYTES_SK);
    printf("  Ciphertext Size     : %lu bytes\r\n", (unsigned long)BYTES_CT);
    printf("  Shared Secret Size  : %lu bytes\r\n\r\n", (unsigned long)BYTES_SS);
    
    sk = (unsigned char *) calloc(BYTES_SK, sizeof(unsigned char));
    pk = (unsigned char *) calloc(BYTES_PK, sizeof(unsigned char));
    ct = (unsigned char *) calloc(BYTES_CT, sizeof(unsigned char));
    ss = (unsigned char *) calloc(BYTES_SS, sizeof(unsigned char));

    if (!sk || !pk || !ct || !ss) {
        printf("Memory allocation failed\r\n");
    }
    else{
        printf("Correct memory allocation\r\n");
    }
    
     if (saber_keypair(pk, sk) != 0) {
        printf("Keypair generation failed\n");
    }
    printf("Keypair generation succesful\n");

    if (saber_encapsulate(ct, ss, pk) != 0) {
            printf("Encapsulation failed\n");
    }
    printf("Correct encapsulation\n");
    
    if (saber_decapsulate(ss, ct, sk) != 0) {
            printf("Encapsulation failed\n");
    }
    
    printf("Correct decapsulation\n");
    
    // Your main application loop
    for(;;)
    {

    }
    
    // Cleanup (never reached in infinite loop)
    free(sk);
    free(pk);
    free(ct);
    free(ss);
}
/* [] END OF FILE */

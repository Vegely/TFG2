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
#include "FreeRTOSConfig.h"
#include "task.h"
#include "timers.h"
#include <stdio.h>
#include "../Serial/uartTask.h"
#include "../saber_wrapper.h"
#include <stdlib.h>

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    
    /* Create RTOS tasks and start the scheduler */
    xTaskCreate ( uartTask, "UART Task", 400, 0, 2, 0);
    vTaskStartScheduler();
    
    const size_t BYTES_SK = saber_get_secret_key_size();
    const size_t BYTES_PK = saber_get_public_key_size();
    const size_t BYTES_CT = saber_get_ciphertext_size();
    const size_t BYTES_SS = saber_get_shared_secret_size();

    unsigned char *sk = (unsigned char *) calloc(BYTES_SK, sizeof(unsigned char));
    unsigned char *pk = (unsigned char *) calloc(BYTES_PK, sizeof(unsigned char));
    unsigned char *ct = (unsigned char *) calloc(BYTES_CT, sizeof(unsigned char));
    unsigned char *ss = (unsigned char *) calloc(BYTES_SS, sizeof(unsigned char));

    if (!sk || !pk || !ct || !ss) {
        printf("Memory allocation failed\n");
    }
    
    for(;;)
    {
        /* Place your application code here. */
    }
    
    free(sk);
    free(pk);
    free(ct);
    free(ss);
}

/* [] END OF FILE */

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
#include <stdio.h>
#include <stdlib.h>
#include "../saber_wrapper.h"


#define RX_BUFFER_SIZE 128
volatile uint8_t rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxHead = 0;
volatile uint16_t rxTail = 0;


unsigned char *sk = NULL;
unsigned char *pk = NULL;
unsigned char *ct = NULL;
unsigned char *ss = NULL;

static void UART_Isr(void)
{
    /* Check for errors or status if necessary, then read FIFO */
    /* Keep reading while HW FIFO has data */
    while(Cy_SCB_UART_GetNumInRxFifo(UART_HW))
    {
        uint32_t receivedData = Cy_SCB_UART_Get(UART_HW);
        
        /* Store in Ring Buffer */
        uint16_t nextHead = (rxHead + 1) % RX_BUFFER_SIZE;
        if(nextHead != rxTail) 
        {
            rxBuffer[rxHead] = (uint8_t)receivedData;
            rxHead = nextHead;
        }
        /* Else: Buffer overflow - drop character or handle error */
    }

    /* Clear Interrupts */
    Cy_SCB_ClearRxInterrupt(UART_HW, CY_SCB_RX_INTR_NOT_EMPTY);
    NVIC_ClearPendingIRQ((IRQn_Type) UART_SCB_IRQ_cfg.intrSrc);
}

int UART_GetCharArray(void)
{
    if(rxHead == rxTail)
    {
        return -1; 
    }
    
    int data = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
    return data;
}
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    
    /* Create RTOS tasks and start the scheduler */
    (void) Cy_SysInt_Init(&UART_SCB_IRQ_cfg, &UART_Isr); 
    NVIC_EnableIRQ((IRQn_Type) UART_SCB_IRQ_cfg.intrSrc);
    Cy_SCB_SetRxInterruptMask(UART_HW, CY_SCB_RX_INTR_NOT_EMPTY);
    
    UART_Start();
    setvbuf(stdin, NULL, _IONBF, 0); // No buffering for printf
    
    printf("\r\n================================\r\n");
    printf(" System Start (Bare Metal Mode)\r\n");
    printf("================================\r\n");

    /* 2. Run Heavy HQC Logic (Previously MainTask) */
    
    // Initialize SABER/HQC
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
    
    /* Allocation */
    sk = (unsigned char *) calloc(BYTES_SK, sizeof(unsigned char));
    pk = (unsigned char *) calloc(BYTES_PK, sizeof(unsigned char));
    ct = (unsigned char *) calloc(BYTES_CT, sizeof(unsigned char));
    ss = (unsigned char *) calloc(BYTES_SS, sizeof(unsigned char));

    if (!sk || !pk || !ct || !ss) {
        printf("CRITICAL: Memory allocation failed. Increase Heap Size in System Settings!\r\n");
        // Trap here or handle error
    } else {
        printf("Memory allocated successfully.\r\n");
        
        // Keygen
        if (saber_keypair(pk, sk) != 0) printf("Keypair generation failed\r\n");
        else printf("Keypair generation successful\r\n");

        // Encaps
        if (saber_encapsulate(ct, ss, pk) != 0) printf("Encapsulation failed\r\n");
        else printf("Encapsulation successful\r\n");
        
        // Decaps
        if (saber_decapsulate(ss, ct, sk) != 0) printf("Decapsulation failed\r\n");
        else printf("Decapsulation successful\r\n");
    }

    // Cleanup crypto memory if not needed anymore, or keep it if reused
    free(sk); free(pk); free(ct); free(ss);
    
    printf("\r\nCrypto done. Entering Command Loop.\r\n");
    
        
    for(;;)
    {

    }
   
}

/* [] END OF FILE */

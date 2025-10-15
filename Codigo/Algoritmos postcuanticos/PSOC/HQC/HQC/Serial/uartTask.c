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
#include "semphr.h"
#include <stdio.h>

static SemaphoreHandle_t uartSemaphore;

/* UART interrupt handler */
static void UART_Isr()
{
    // Disable & clear the interrupt
    Cy_SCB_SetRxInterruptMask(UART_HW,0);
    Cy_SCB_ClearRxInterrupt(UART_HW, CY_SCB_RX_INTR_NOT_EMPTY);
    NVIC_ClearPendingIRQ((IRQn_Type) UART_SCB_IRQ_cfg.intrSrc);

    // If the semaphore causes a task switch you should yield to that task
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(uartSemaphore,&xHigherPriorityTaskWoken); // Tell the UART thread to process the RX FIFO
    if(xHigherPriorityTaskWoken != pdFALSE)
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/* UART task */
void uartTask(void *arg)
{
    (void)arg;
    UART_Start();
    setvbuf( stdin, NULL, _IONBF, 0 ); // Disable STDIN input buffering
    printf("Started Task\r\n");

    /* Create a semaphore. It will be set in the UART ISR when data is available */
    uartSemaphore = xSemaphoreCreateBinary();    
 
    /* Configure the interrupt and interrupt handler for the UART */
    (void) Cy_SysInt_Init(&UART_SCB_IRQ_cfg, &UART_Isr); 
    NVIC_EnableIRQ((IRQn_Type) UART_SCB_IRQ_cfg.intrSrc);
    Cy_SCB_SetRxInterruptMask(UART_HW,CY_SCB_RX_INTR_NOT_EMPTY);
    
    while(1)
    {
        /* Wait here until the semaphore is given (i.e. set) by the ISR */
        xSemaphoreTake(uartSemaphore,portMAX_DELAY);

        /* Process all of hte characters in the command buffer */
        while(Cy_SCB_UART_GetNumInRxFifo(UART_HW))
        {
            char c= getchar();
            switch(c)
            {
                case '?':
                    printf("?\tPrint Help\r\n");
                break;
            }
        }
        // Once you have processed the RX FIFO reenable the interrupt
        Cy_SCB_SetRxInterruptMask(UART_HW,CY_SCB_RX_INTR_NOT_EMPTY);
    }
}
/* [] END OF FILE */

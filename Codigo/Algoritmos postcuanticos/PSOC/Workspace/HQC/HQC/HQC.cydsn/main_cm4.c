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
#include "mainTask.h"



volatile uint8_t UART_READY=0;


int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    
    /* Create RTOS tasks and start the scheduler */
    xTaskCreate(uartTask, "UART Task", 400, 0, 2, 0);
    xTaskCreate(mainTask, "Main Task", 2*8192, 0, 1, 0);
    vTaskStartScheduler();
    
        
    for(;;)
    {

    }
   
}

/* [] END OF FILE */

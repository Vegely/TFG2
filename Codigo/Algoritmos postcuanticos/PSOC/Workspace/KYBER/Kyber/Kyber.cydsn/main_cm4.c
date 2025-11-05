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
#include "../kyber_wrapper.h"
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    xTaskCreate ( uartTask, "UART Task", 400, 0, 2, 0);
    vTaskStartScheduler(); // This function never returns

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */

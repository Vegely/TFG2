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
#include "uartTask.h"



int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    /* Create RTOS tasks and start the scheduler */
    xTaskCreate ( uartTask, "UART Task", 400, 0, 2, 0);
    vTaskStartScheduler(); // This function never returns
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/*void LED_TASK(void){
    for(;;)
    {
        Cy_GPIO_Write(RED_PORT,RED_NUM,1);
        vTaskDelay(500);
        Cy_GPIO_Write(RED_PORT,RED_NUM,0);
        vTaskDelay(500);
    }
}*/
//xTaskCreate(LED_TASK,"LED_TAST",400,NULL,1,0);
//vTaskStartScheduler();

//int main(void)
//{
//    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
//    UART_Start();
//    setvbuf(stdin,NULL,_IONBF,0); //No buffer
//    char c;
//    printf("Started UART\r\n");
//    for(;;)
//    {
//        c=getchar();
//        if(c){
//            printf("%c", c);
//        }
//    }
//}

/* [] END OF FILE */

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
#pragma once

#include "semphr.h"

void uartTask(void *arg);

// Semaphore that signals when UART is initialized and ready
extern SemaphoreHandle_t uartReadySemaphore;
/* [] END OF FILE */


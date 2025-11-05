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
#include "cycle_counter.h"

void CycleCounter_Init(void)
{
    /* Enable the trace and debug blocks */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* Reset the cycle counter */
    DWT->CYCCNT = 0;

    /* Enable the cycle counter */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t CycleCounter_Get(void)
{
    return DWT->CYCCNT;
}

/* [] END OF FILE */

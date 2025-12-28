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
#ifndef CYCLE_COUNTER_H
#define CYCLE_COUNTER_H

#include <stdint.h>
#include "cy_device_headers.h"

/** Initialize and enable DWT cycle counter */
void CycleCounter_Init(void);

/** Get current CPU cycle count */
uint32_t CycleCounter_Get(void);

/** Convenience macros for readability */
#define CycleCounter_Start()   CycleCounter_Get()
#define CycleCounter_End()     CycleCounter_Get()

#ifdef __cplusplus
}
#endif

#endif /* CYCLE_COUNTER_H */

/* [] END OF FILE */

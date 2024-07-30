#ifndef __TOOLS_H
#define __TOOLS_H
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void DWT_Init(void);

#ifdef __cplusplus
}
#endif

#endif
#include "ErrorHandler.h"
#include <stm32f4xx_hal.h>

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
extern "C" void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq(); // 禁用中断
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

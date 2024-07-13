#ifndef EASYUART_H
#define EASYUART_H

#include "stm32f4xx_hal.h"
#include <string>
#include <ErrorHandler.h>

// 宏定义
#define UART_BUFFER_SIZE 256

// 常量定义
extern const char del[];
extern const char newline[];

extern UART_HandleTypeDef huart4; // 声明为 extern


// 变量声明
extern uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
extern volatile uint8_t uart_rx_char;
extern volatile uint8_t uart_rx_index;
extern volatile uint8_t uart_rx_complete;

// 函数声明
extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
void UART_SendString(UART_HandleTypeDef* huart, uint8_t* data);
void uart_print(const uint8_t* message, const uint8_t* end = (const uint8_t*)newline, UART_HandleTypeDef* huart = &huart4);
void uart_print(const char* message, const char* end = "\r\n", UART_HandleTypeDef* huart = &huart4);
void uart_print(const std::string& message, const std::string& end = "\r\n", UART_HandleTypeDef* huart = &huart4);
void uart_input_it(uint8_t *buffer, size_t max_len);
void MX_UART4_Init(void);

#endif // EASYUART_H
#ifndef EASYUART_H
#define EASYUART_H

#include "stm32f4xx_hal.h"
#include <ErrorHandler.h>
#include <string>


// 宏定义
#define UART_BUFFER_SIZE 256

// 常量定义
extern const char del[];
extern const char newline[];

#define UART_NONE "\e[0m"
#define UART_BLACK "\e[0;30m"
#define UART_L_BLACK "\e[1;30m"
#define UART_RED "\e[0;31m"
#define UART_L_RED "\e[1;31m"
#define UART_GREEN "\e[0;32m"
#define UART_L_GREEN "\e[1;32m"
#define UART_BROWN "\e[0;33m"
#define UART_YELLOW "\e[1;33m"
#define UART_BLUE "\e[0;34m"
#define UART_L_BLUE "\e[1;34m"
#define UART_PURPLE "\e[0;35m"
#define UART_L_PURPLE "\e[1;35m"
#define UART_CYAN "\e[0;36m"
#define UART_L_CYAN "\e[1;36m"
#define UART_GRAY "\e[0;37m"
#define UART_WHITE "\e[1;37m"

#define UART_BOLD "\e[1m"
#define UART_UNDERLINE "\e[4m"
#define UART_BLINK "\e[5m"
#define UART_REVERSE "\e[7m"
#define UART_HIDE "\e[8m"
#define UART_CLEAR "\e[2J"
#define UART_CLRLINE "\r\e[K" // or "\e[1K\r"

extern UART_HandleTypeDef huart4; // 声明为 extern

// 变量声明
extern uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
extern volatile uint8_t uart_rx_char;
extern volatile uint8_t uart_rx_index;
extern volatile uint8_t uart_rx_complete;
extern volatile bool uart_term_debug;
extern volatile bool uart_ad9220_debug;

// 函数声明
extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
void UART_SendString(UART_HandleTypeDef* huart, uint8_t* data);
void uart_print(const uint8_t* message,
                const uint8_t* end = (const uint8_t*)newline,
                UART_HandleTypeDef* huart = &huart4);
void uart_print(const char* message,
                const char* end = "\r\n",
                UART_HandleTypeDef* huart = &huart4);
void uart_print(const std::string& message,
                const std::string& end = "\r\n",
                UART_HandleTypeDef* huart = &huart4);
void uart_print(const double message,
                const char* end = "\r\n",
                UART_HandleTypeDef* huart = &huart4);
void uart_print(const float message,
                const char* end = "\r\n",
                UART_HandleTypeDef* huart = &huart4);
void uart_input_it(uint8_t* buffer, size_t max_len);
void MX_UART4_Init(void);

void uart_log_trace(const char* message,
                    const char* end = "\r\n",
                    UART_HandleTypeDef* huart = &huart4);
void uart_log_debug(const char* message,
                    const char* end = "\r\n",
                    UART_HandleTypeDef* huart = &huart4);
void uart_log_info(const char* message,
                   const char* end = "\r\n",
                   UART_HandleTypeDef* huart = &huart4);
void uart_log_warn(const char* message,
                   const char* end = "\r\n",
                   UART_HandleTypeDef* huart = &huart4);
void uart_log_error(const char* message,
                    const char* end = "\r\n",
                    UART_HandleTypeDef* huart = &huart4);
void uart_log_success(const char* message,
                      const char* end = "\r\n",
                      UART_HandleTypeDef* huart = &huart4);

std::string doubleToString(double value, uint8_t precision);
std::string floatToString(double value, uint8_t precision);

// 函数指针类型和默认实现
typedef void (*FunctionType)();
void defaultRxCallbackStart();
void defaultRxCallbackEnd();
void defaultTxCallbackStart();
void defaultTxCallbackEnd();

extern FunctionType usedRxCallbackStart;
extern FunctionType usedRxCallbackEnd;
extern FunctionType usedTxCallbackStart;
extern FunctionType usedTxCallbackEnd;

#endif // EASYUART_H
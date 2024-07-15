#include "EasyUART.h"
#include <string.h>

// 定义常量
const char del[] = "\b \b";
const char newline[] = "\r\n";

// 定义变量
uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_rx_char;
volatile uint8_t uart_rx_index = 0;
volatile uint8_t uart_rx_complete = 0;




// 默认回调函数
void defaultRxCallbackStart() {
    // 默认实现
}

void defaultRxCallbackEnd() {
    // 默认实现
}

void defaultTxCallbackStart() {
    // 默认实现
}

void defaultTxCallbackEnd() {
    // 默认实现
}

// 函数指针初始化为默认回调函数
FunctionType usedRxCallbackStart = defaultRxCallbackStart;
FunctionType usedRxCallbackEnd = defaultRxCallbackEnd;
FunctionType usedTxCallbackStart = defaultTxCallbackStart;
FunctionType usedTxCallbackEnd = defaultTxCallbackEnd;

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    if (huart->Instance == UART4) {
        usedRxCallbackStart();
        switch (uart_rx_char) {
        case '\r':
            usedTxCallbackStart();
            HAL_UART_Transmit(huart, (uint8_t*)newline, strlen(newline), HAL_MAX_DELAY);
            uart_rx_buffer[uart_rx_index] = '\0';
            uart_rx_complete = 1;
            uart_rx_index = 0;
            usedTxCallbackEnd();
            break;

        case '\b':
        case 127:
            if (uart_rx_index > 0) {
                usedTxCallbackStart();
                uart_rx_index--;
                HAL_UART_Transmit(huart, (uint8_t*)del, strlen(del), HAL_MAX_DELAY);
                usedTxCallbackEnd();
            }
            break;

        default:
            if (uart_rx_index >= UART_BUFFER_SIZE - 1) {
                uart_rx_buffer[uart_rx_index] = '\0';
                uart_rx_complete = 1;
                uart_rx_index = 0;
            } else {
                usedTxCallbackStart();
                HAL_UART_Transmit(huart, (uint8_t*)&uart_rx_char, 1, HAL_MAX_DELAY);
                uart_rx_buffer[uart_rx_index] = uart_rx_char;
                uart_rx_index++;
                usedTxCallbackEnd();
            }
            break;
        }
        HAL_UART_Receive_IT(&huart4, (uint8_t*)&uart_rx_char, 1);
        usedRxCallbackEnd();
    }
}

void UART_SendString(UART_HandleTypeDef* huart, uint8_t* data) {
    HAL_UART_Transmit(huart, data, strlen((char*)data), HAL_MAX_DELAY);
}

void uart_print(const uint8_t* message, const uint8_t* end, UART_HandleTypeDef* huart) {
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    usedTxCallbackEnd();
}

void uart_print(const char* message, const char* end, UART_HandleTypeDef* huart) {
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    usedTxCallbackEnd();
}

void uart_print(const std::string& message, const std::string& end, UART_HandleTypeDef* huart) {
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)message.c_str());
    UART_SendString(huart, (uint8_t*)end.c_str());
    usedTxCallbackEnd();
}

void uart_log_trace(const char* message, const char* end, UART_HandleTypeDef* huart){
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)GRAY);
    UART_SendString(huart, (uint8_t*)"Trace  | ");
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    UART_SendString(huart, (uint8_t*)NONE);
    usedTxCallbackEnd();
}
void uart_log_debug(const char* message, const char* end, UART_HandleTypeDef* huart){
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)BLUE);
    UART_SendString(huart, (uint8_t*)"Debug   | ");
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    UART_SendString(huart, (uint8_t*)NONE);
    usedTxCallbackEnd();
}
void uart_log_info(const char* message, const char* end, UART_HandleTypeDef* huart){
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)WHITE);
    UART_SendString(huart, (uint8_t*)"Info    | ");
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    UART_SendString(huart, (uint8_t*)NONE);
    usedTxCallbackEnd();
}
void uart_log_warn(const char* message, const char* end, UART_HandleTypeDef* huart){
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)BROWN);
    UART_SendString(huart, (uint8_t*)"Warn    | ");
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    UART_SendString(huart, (uint8_t*)NONE);
    usedTxCallbackEnd();
}
void uart_log_error(const char* message, const char* end, UART_HandleTypeDef* huart){
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)RED);
    UART_SendString(huart, (uint8_t*)"Error   | ");
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    UART_SendString(huart, (uint8_t*)NONE);
    usedTxCallbackEnd();
}
void uart_log_success(const char* message, const char* end, UART_HandleTypeDef* huart){
    usedTxCallbackStart();
    UART_SendString(huart, (uint8_t*)GREEN);
    UART_SendString(huart, (uint8_t*)"Success | ");
    UART_SendString(huart, (uint8_t*)message);
    UART_SendString(huart, (uint8_t*)end);
    UART_SendString(huart, (uint8_t*)NONE);
    usedTxCallbackEnd();
}


void uart_input_it(uint8_t *buffer, size_t max_len) {
    uart_rx_index = 0;
    while (!uart_rx_complete) {
        // 等待接收完成
    }
    strncpy((char*)buffer, (char*)uart_rx_buffer, max_len); // 复制接收到的字符串到 buffer
    buffer[max_len - 1] = '\0';               // 确保以 '\0' 结尾

    uart_rx_complete = 0; // 重置接收完成标志
}

void MX_UART4_Init(void) {
    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart4) != HAL_OK) {
        Error_Handler(); // 如果初始化失败，调用错误处理函数
    }
    // 启用UART接收中断
    HAL_UART_Receive_IT(&huart4, (uint8_t*)&uart_rx_char, 1);
}
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include <cstring>
#include <string.h>
#include <string>


#include<EasyLed.h>

#define UART_BUFFER_SIZE 256
const char del[] = "\b \b";
const char newline[] = "\r\n";

uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_rx_char;
volatile uint8_t uart_rx_index = 0;
volatile uint8_t uart_rx_complete = 0;

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim8;        // TIM8 定时器句柄
DMA_HandleTypeDef hdma_tim8_up; // TIM8 DMA 通道句柄
UART_HandleTypeDef huart4;      // UART4 外设句柄

/* Private function prototypes -----------------------------------------------*/
extern "C" void SystemClock_Config(void); // 系统时钟配置函数
static void MX_GPIO_Init(void);           // GPIO 初始化函数
static void MX_DMA_Init(void);            // DMA 初始化函数
static void MX_TIM8_Init(void);           // TIM8 初始化函数
static void MX_UART4_Init(void);          // UART4 初始化函数

/* Private user code ---------------------------------------------------------*/



EasyLed blue_led;
EasyLed red_led;




void uart_input_it(uint8_t *buffer, size_t max_len)
{
    uart_rx_index = 0;
    // Enable UART receive interrupt
    // HAL_UART_Receive_IT(&huart4, &uart_rx_char, 1);
    while (!uart_rx_complete)
    {
        // 等待接收完成
    }
    strncpy((char*)buffer,(char*)uart_rx_buffer, max_len); // 复制接收到的字符串到 buffer
    buffer[max_len - 1] = '\0';               // 确保以 '\0' 结尾

    uart_rx_complete = 0; // 重置接收完成标志
}

/**
 * @brief  The application entry point.
 * @retval int
 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
extern "C" void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage */
	__HAL_RCC_PWR_CLK_ENABLE(); // 启用电源时钟
	__HAL_PWR_VOLTAGESCALING_CONFIG(
	    PWR_REGULATOR_VOLTAGE_SCALE1); // 配置电压调节器输出电压

	/** Initializes the RCC Oscillators according to the specified parameters in
	 * the RCC_OscInitTypeDef structure. */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 144;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler(); // 如果配置失败，调用错误处理函数
	}

	/** Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
	                              RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler(); // 如果配置失败，调用错误处理函数
	}
}

/**
 * @brief TIM8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM8_Init(void) {

	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 0;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 10 - 1;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_PWM_Init(&htim8) != HAL_OK) {
		Error_Handler(); // 如果初始化失败，调用错误处理函数
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) !=
	    HAL_OK) {
		Error_Handler(); // 如果配置失败，调用错误处理函数
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = 5;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) !=
	    HAL_OK) {
		Error_Handler(); // 如果配置失败，调用错误处理函数
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) !=
	    HAL_OK) {
		Error_Handler(); // 如果配置失败，调用错误处理函数
	}

	HAL_TIM_MspPostInit(&htim8); // TIM8 后初始化
}

void Start_PWM(void) {
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1); // 启动TIM8通道1的PWM输出
}

/**
 * @brief UART4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART4_Init(void) {

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

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA2_CLK_ENABLE(); // 启用 DMA2 时钟

	/* DMA interrupt init */
	/* DMA2_Stream1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0,
	                     0); // 设置 DMA2 Stream1 中断优先级
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn); // 使能 DMA2 Stream1 中断
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE(); // 启用 GPIOH 时钟
	__HAL_RCC_GPIOA_CLK_ENABLE(); // 启用 GPIOA 时钟
	__HAL_RCC_GPIOD_CLK_ENABLE(); // 启用 GPIOD 时钟
	__HAL_RCC_GPIOC_CLK_ENABLE(); // 启用 GPIOC 时钟

	/* Configure GPIO pin: PA5 */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //! 初始化 PA5 引脚为 TIM8 PWM 输出

	/*Configure GPIO pins : PD8 PD9 PD10 PD11
	                         PD12 PD0 PD1 PD2
	                         PD3 PD4 PD5 PD6
	                         PD7 */
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
	                      GPIO_PIN_12 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
	                      GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
	                      GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); // 初始化 GPIOD 引脚
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
extern "C" void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state
	 */
	__disable_irq(); // 禁用中断
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number where
 * the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line
	 * number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
	 * file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
	if (huart->Instance == UART4) {
		blue_led.blink();
		switch (uart_rx_char) {
		case '\r':
			HAL_UART_Transmit(huart, (uint8_t*)newline, strlen(newline),
			                  HAL_MAX_DELAY);
			uart_rx_buffer[uart_rx_index] = '\0';
			uart_rx_complete = 1;
			uart_rx_index = 0;
			break;

		case '\b':
		case 127:
			if (uart_rx_index > 0) {
				uart_rx_index--;
				HAL_UART_Transmit(huart, (uint8_t*)del, strlen(del),
				                  HAL_MAX_DELAY);
			}
			break;

		default:
			if (uart_rx_index >= UART_BUFFER_SIZE - 1) {
				uart_rx_buffer[uart_rx_index] = '\0';
				uart_rx_complete = 1;
				uart_rx_index = 0;
			} else {
				HAL_UART_Transmit(huart, (uint8_t*)&uart_rx_char, 1,
				                  HAL_MAX_DELAY);
				uart_rx_buffer[uart_rx_index] = uart_rx_char;
				uart_rx_index++;
			}
			break;
		}
		HAL_UART_Receive_IT(&huart4, (uint8_t*)&uart_rx_char, 1);
	}
}

void UART_SendString(UART_HandleTypeDef* huart, uint8_t* data) {
	HAL_UART_Transmit(huart, data, strlen((char*)data), HAL_MAX_DELAY);
	red_led.blink();
}

void uart_print(const uint8_t* message,
                const uint8_t* end = (const uint8_t*)newline,
                UART_HandleTypeDef* huart = &huart4) {
	UART_SendString(huart, (uint8_t*)message);
	UART_SendString(huart, (uint8_t*)end);
}

void uart_print(const char* message,
                const char* end = "\r\n",
                UART_HandleTypeDef* huart = &huart4) {
	UART_SendString(huart, (uint8_t*)message);
	UART_SendString(huart, (uint8_t*)end);
}

void uart_print(const std::string& message,
                const std::string& end = "\r\n",
                UART_HandleTypeDef* huart = &huart4) {
	UART_SendString(huart, (uint8_t*)message.c_str());
	UART_SendString(huart, (uint8_t*)end.c_str());
}

extern "C" int main(void) {

	HAL_Init(); // 初始化 HAL 库

	SystemClock_Config(); // 配置系统时钟

	/* Initialize all configured peripherals */
	MX_GPIO_Init(); // 初始化 GPIO
	red_led.init(GPIOC, GPIO_PIN_5);
	blue_led.init(GPIOB, GPIO_PIN_2);
	MX_DMA_Init();   // 初始化 DMA
	MX_TIM8_Init();  // 初始化 TIM8
	MX_UART4_Init(); // 初始化 UART4
	Start_PWM();     //! 初始化PA5上的PWM输出
	uart_print("hello world");

	while (1) {
		uart_print(".");

		// red_led.switchOn();
		HAL_Delay(10);

		// red_led.switchOff();
		// HAL_Delay(100);

		uint8_t input_buffer[100];
		uart_input_it(input_buffer, sizeof(input_buffer));
		uart_print("Received: "); // 打印接收到的数据
		uart_print(input_buffer);
	}
}
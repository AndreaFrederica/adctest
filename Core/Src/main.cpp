#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include <EasyLed.h>
#include <EasyUART.h>
#include <ErrorHandler.h>
#include <cmath>
#include <cstring>
#include <string>

extern void Error_Handler(void);

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim8;        // TIM8 定时器句柄
DMA_HandleTypeDef hdma_tim8_up; // TIM8 DMA 通道句柄
UART_HandleTypeDef huart4;      // UART4 外设句柄

/* Private function prototypes -----------------------------------------------*/
extern "C" void SystemClock_Config(void); // 系统时钟配置函数
static void MX_GPIO_Init(void);           // GPIO 初始化函数
static void MX_DMA_Init(void);            // DMA 初始化函数
static void MX_TIM8_Init(void);           // TIM8 初始化函数

/* Private user code ---------------------------------------------------------*/

const int sys_frequency = 144; //? mHz

EasyLed blue_led;
EasyLed red_led;
int tim8_set_prescaler = 1000; //? 定时器的预分频器
int tim8_set_period = 10;      //? 9+1 定时器的周期

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
	RCC_OscInitStruct.PLL.PLLN = sys_frequency;
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
	htim8.Init.Prescaler = tim8_set_prescaler; //! Default:1000 -> 14.5kHz
	// htim8.Init.Prescaler = 0;

	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = tim8_set_period; //? (10-1)
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_PWM_Init(&htim8) != HAL_OK) {
		Error_Handler(); // 如果初始化失败，调用错误处理函数
		uart_log_error("tim8 init pass1 start failed");
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) !=
	    HAL_OK) {
		Error_Handler(); // 如果配置失败，调用错误处理函数
		uart_log_error("tim8 init pass2 start failed");
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
		uart_print("Error | tim8 init pass3 start failed");
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
		uart_log_error("tim8 init pass4 start failed");
	}
	__HAL_RCC_TIM8_CLK_ENABLE();
	HAL_TIM_MspPostInit(&htim8); // TIM8 后初始化
}

void Start_PWM(void) {
	// HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1); // 启动TIM8通道1的PWM输出
	//? 互补输出和PWM输出是两个输出信号
	if (HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_1) != HAL_OK) {
		//! PA5是互补输出
		uart_log_error("pwm start failed");
	} else {
		uart_log_success("pwm start succeed");
	}
	// TIM8->CCR1 = 16383;
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
	__HAL_RCC_GPIOB_CLK_ENABLE(); // 启用 GPIOA 时钟
	__HAL_RCC_GPIOD_CLK_ENABLE(); // 启用 GPIOD 时钟
	__HAL_RCC_GPIOC_CLK_ENABLE(); // 启用 GPIOC 时钟

	// /* Configure GPIO pin: PA5 */
	// GPIO_InitStruct.Pin = GPIO_PIN_5;
	// GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	// GPIO_InitStruct.Pull = GPIO_NOPULL;
	// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	// GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
	// HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	//! 初始化 PA5 引脚为 TIM8 PWM输出
	//! 相关初始化代码在HAL中已经存在

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

void uartRxCallbackStart() { red_led.switchOn(); }
void uartRxCallbackEnd() { red_led.switchOff(); }
void uartTxCallbackStart() { blue_led.switchOn(); }
void uartTxCallbackEnd() { blue_led.switchOff(); }

u_int16_t ad9020Read() {
	uint16_t data = 0;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0)  << 0;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)  << 1;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2)  << 2;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3)  << 3;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4)  << 4;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_5)  << 5;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6)  << 6;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7)  << 7;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8)  << 8;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)  << 9;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) << 10;
    data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11) << 11;
	data |= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12) << 12;
	return data;
}


extern "C" int main(void) {

	HAL_Init(); // 初始化 HAL 库

	SystemClock_Config(); // 配置系统时钟

	/* Initialize all configured peripherals */
	MX_GPIO_Init(); // 初始化 GPIO
	red_led.init(GPIOC, GPIO_PIN_5);
	blue_led.init(GPIOB, GPIO_PIN_2);

	usedRxCallbackStart = uartRxCallbackStart;
	usedRxCallbackEnd = uartRxCallbackEnd;
	usedTxCallbackStart = uartTxCallbackStart;
	usedTxCallbackEnd = uartTxCallbackEnd;
	MX_UART4_Init(); // 初始化 UART4
	uart_print("\n");
	uart_log_info("hello stm32f407");
	uart_log_info("init dma");
	MX_DMA_Init(); // 初始化 DMA
	uart_log_info("init tim8");
	MX_TIM8_Init(); // 初始化 TIM8
	uart_log_info("init pwm output");
	Start_PWM(); //! 初始化PA5上的PWM输出

	uart_log_success("init done");
	uart_print("hello world");

	while (1) {
		if (uart_ad9220_debug == false) {
			uint8_t input_buffer[100];
			uart_print("new tim8_set_period:", "");
			uart_input_it(input_buffer, sizeof(input_buffer));
			uart_print("Received: "); // 打印接收到的数据
			uart_print(input_buffer);

			// 检查是否包含 "reboot"
			if (strstr((char*)input_buffer, "reboot") != nullptr) {
				reboot();
			}
			char number_buffer[100] = {0}; // 用来存储提取出的数字
			getNumberChar(input_buffer, number_buffer);

			uart_print("Numbers Extracted: ");
			uart_print(number_buffer);

			//? 将提取的数字转换为 int 类型
			int extracted_number = atoi(number_buffer);
			// uart_print("Converted to int: " +
			// std::to_string(extracted_number));
			tim8_set_period = extracted_number; //! 设置pwm频率
			//? 使用公式 y = sys_frequency / （tim8_set_period x
			//tim8_set_prescaler+1） 计算预计的PWM频率
			double frequency = 0;
			frequency = ((sys_frequency * 1000000) /
			             ((tim8_set_prescaler + 1) * tim8_set_period)) /
			            1000;
			uart_print("Calculated Frequency: " + doubleToString(frequency, 4) +
			           " kHz");

			uart_print("new tim8_set_prescaler:", "");
			uart_input_it(input_buffer, sizeof(input_buffer));
			uart_print("Received: "); // 打印接收到的数据
			uart_print(input_buffer);

			// 检查是否包含 "reboot"
			if (strstr((char*)input_buffer, "reboot") != nullptr) {
				reboot();
			}
			number_buffer[100] = {0}; // 用来存储提取出的数字
			getNumberChar(input_buffer, number_buffer);

			uart_print("Numbers Extracted: ");
			uart_print(number_buffer);

			//? 将提取的数字转换为 int 类型
			extracted_number = atoi(number_buffer);
			// uart_print("Converted to int: " +
			// std::to_string(extracted_number));
			tim8_set_prescaler = extracted_number; //! 设置pwm频率
			//? 使用公式 y = sys_frequency / （tim8_set_period x
			//tim8_set_prescaler+1） 计算预计的PWM频率 double frequency = 0;
			frequency = ((sys_frequency * 1000000) /
			             ((tim8_set_prescaler + 1) * tim8_set_period)) /
			            1000;
			uart_print("Calculated Frequency: " + doubleToString(frequency, 4) +
			           " kHz");

			MX_TIM8_Init();
			Start_PWM();
		}else{
			uint16_t number = ad9020Read();
			char str[6];
			ConvertUint16ToString(number, str);
			uart_print(str);
		}
	}
}

void getNumberChar(uint8_t input_buffer[100], char number_buffer[100]) {
	int j = 0;
	for (int i = 0; i < sizeof(input_buffer); i++) {
		if (isdigit(input_buffer[i])) {
			number_buffer[j++] = input_buffer[i];
		}
	}
}

void reboot() {
	uart_log_warn("Rebooting MCU...");
	blue_led.switchOn();
	HAL_Delay(100);

	// 执行 MCU 重启操作，可以通过适当的函数实现
	NVIC_SystemReset();
}

void ConvertUint16ToString(uint16_t num, char *str) {
    sprintf(str, "%u", num); // 将 uint16_t 转换为字符串
}
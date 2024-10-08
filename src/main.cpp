#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include <AndreaUI.h>
#include <EasyLCD.h>
#include <EasyLed.h>
#include <EasySPI.h>
#include <EasyUART.h>
#include <ErrorHandler.h>
#include <GUI.h>
#include <cmath>
#include <cstring>
#include <ec11.h>
#include <string>
#include <tools.h>

extern "C" {
//#include <PE43xx.h>
#include <ad9959.h>

}

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

DAC_HandleTypeDef hdac;

void DAC_Init(void) {
	__HAL_RCC_DAC_CLK_ENABLE(); // 使能DAC时钟

	hdac.Instance = DAC;
	if (HAL_DAC_Init(&hdac) != HAL_OK) {
		uart_log_error("init dac fail pass1");
	} else {
		uart_log_success("init dac success pass1");
	}

	DAC_ChannelConfTypeDef sConfig = {0};
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;             // 选择触发源
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // 启用输出缓冲

	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
		// 配置通道错误处理
		uart_log_error("init dac fail pass2");
	} else {
		uart_log_success("init dac success pass2");
	}

	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK) {
		// 配置通道错误处理
		uart_log_error("init dac fail pass3");
	} else {
		uart_log_success("init dac success pass3");
	}
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (dacHandle->Instance == DAC) {
		__HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIO时钟

		// 配置PA4和PA5引脚为模拟模式
		GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void initDacOutput(void) {
	if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK) {
		// 启动通道1错误处理
		uart_log_error("init DAC channel 1 output fail");
	} else {
		uart_log_success("init DAC channel 1 output success");
	}

	if (HAL_DAC_Start(&hdac, DAC_CHANNEL_2) != HAL_OK) {
		// 启动通道2错误处理
		uart_log_error("init DAC channel 2 output fail");
	} else {
		uart_log_success("init DAC channel 2 output success");
	}
}

void setDacOutput(uint32_t channel, uint32_t value) {
	if (HAL_DAC_SetValue(&hdac, channel, DAC_ALIGN_12B_R, value) != HAL_OK) {
		// 设置值错误处理
		uart_log_error("set DAC value fail");
	} else {
		uart_log_success("set DAC value success");
	}
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
	                      GPIO_PIN_7 | GPIO_PIN_13;
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

void drawSelectLine(Spirit obj) {
	//? 绘制黑色背景
	for (int i = 0; i <= 8 * 16 / 8; i++) {
		obj.P_1BIT[i] = 0;
	}
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < y; x++) {
			obj.setPixel(x, y, true);
		}
	}
	for (int y = 8; y < 15; y++) {
		for (int x = 0; x < (15 - y); x++) {
			obj.setPixel(x, y, true);
		}
	}
}

typedef enum { AM, CW } Enum_prog_output_mode;

int prog_output_mode = AM;
int prog_voltage = 100;         //? 100mV
int prog_amj = 30;              //? 30%
int prog_sd_delay = 50;         //? 50ns
int prog_sd_attenuation = 0;    //?dB
int prog_freq = 30;             //? Mhz
int prog_dm_phase = 0;          //? deg
int prog_wireless_sm_phase = 0; //? deg

int select_y = 0;
bool flag_edit_mode = false;

void initSelectRow(bool update) {
	for (int i = 1; i <= 4; i++) {
		SetChar(i, 0, ' ', BLACK, WHITE);
	}
	SetChar(6, 0, ' ', BLACK, WHITE);
	for (int i = 8; i <= 10; i++) {
		SetChar(i, 0, ' ', BLACK, WHITE);
	}
	if (update) {
		UpdateScreen();
	}
}

void ec11Add() {
	if (flag_edit_mode) {
		switch (select_y) {
		case 0:
			if (prog_output_mode == AM) {
				prog_output_mode = CW;
			} else {
				prog_output_mode = AM;
			}
			break;
		case 1:
			if (prog_voltage < 1000) {
				prog_voltage += 100;
			}
			break;
		case 2:
			if (prog_freq < 40) {
				prog_freq += 1;
			}
			break;
		case 3:
			if (prog_output_mode == AM) {
				if (prog_amj < 90) {
					prog_amj += 10;
				}
			}
			break;
		case 4:
			if (prog_dm_phase < 180) {
				prog_dm_phase += 30;
			}
			break;
		case 5:
			if (prog_sd_delay < 200) {
				prog_sd_delay += 30;
			}
			break;
		case 6:
			if (prog_sd_attenuation < 20) {
				prog_sd_attenuation += 2;
			}
			break;
		case 7:
			if (prog_wireless_sm_phase < 180) {
				prog_wireless_sm_phase += 30;
			}
			break;
			// case 8:
			// 	if (prog_wireless_sd_delay < 200) {
			// 		prog_wireless_sd_delay += 30;
			// 	}
			// 	break;
			// case 9:
			// 	if (prog_wireless_sd_attenuation < 20) {
			// 		prog_wireless_sd_attenuation += 2;
			// 	}
			// 	break;
			// case 10:
			// 	if (prog_wireless_sm_phase < 180) {
			// 		prog_wireless_sm_phase += 10;
			// 	}
			break;
		}
	} else {
		if (select_y >= 7) {
			select_y = 0;
		} else {
			select_y++;
		}
	}
}

void ec11Minus() {
	if (flag_edit_mode) {
		switch (select_y) {
		case 0:
			if (prog_output_mode == AM) {
				prog_output_mode = CW;
			} else {
				prog_output_mode = AM;
			}
			break;
		case 1:
			if (prog_voltage > 100) {
				prog_voltage -= 100;
			}
			break;
		case 2:
			if (prog_freq > 30) {
				prog_freq -= 1;
			}
			break;
		case 3:
			if (prog_output_mode == AM) {
				if (prog_amj > 30) {
					prog_amj -= 10;
				}
			}
			break;
		case 4:
			if (prog_dm_phase > 0) {
				prog_dm_phase -= 30;
			}
			break;
		case 5:
			if (prog_sd_delay > 50) {
				prog_sd_delay -= 30;
			}
			break;
		case 6:
			if (prog_sd_attenuation > 0) {
				prog_sd_attenuation -= 2;
			}
			break;
		case 7:
			if (prog_wireless_sm_phase > 0) {
				prog_wireless_sm_phase -= 30;
			}
			break;
		}
	} else {
		if (select_y <= 0) {
			select_y = 7;
		} else {
			select_y--;
		}
	}
}
void ec11Click() {
	switch (select_y) {
	case 3:
		if (prog_output_mode == AM) {
			if (flag_edit_mode) {
				flag_edit_mode = false;
			} else {
				flag_edit_mode = true;
			}
		}
		break;

	default:
		if (flag_edit_mode) {
			flag_edit_mode = false;
		} else {
			flag_edit_mode = true;
		}
		break;
	}
	progUPDATE();
	// uart_log_debug("EC11_Enter!"); // 按下确认键
}

void progUPDATE() {
	// AD9959_Set_Fre(CH0, prog_wireless_fc_freq * 10000);
	// Write_Frequence(0, prog_freq * 1000000);
	// // IO_Update();
	// setDacOutput(DAC_CHANNEL_1, 2040 * prog_amj / 100);
	// Write_Phase(0, prog_wireless_sm_phase);
	// Write_Amplitude(2, prog_sd_attenuation);
	// Write_Amplitude(3, prog_sd_attenuation);
	// // setDacOutput(DAC_CHANNEL_1, 2048);
	setAD9959();
	// PE4302_0_Set((uint8_t)prog_sd_attenuation);
	// PE43xx_changelevel((float)prog_sd_attenuation);
}

void setAD9959() {
	static int f30_val_bata = 5;       //? 放大倍数
	static int f2_val_bata = 5;        //? 放大倍数
	static uint32_t am_freq = 2000000; //? 2M

	int ch_30M_raw_amplitude = 0; //? 载波原始幅值
	int ch_2M_raw_amplitude = 0;  //? 正弦调制信号原始幅值
	int ch_2M_sm_phase = 0;       //? 对于AM 2M调制信号 使用相位计算
	double amplitudeX = 0;        //? 衰减系数
	//! 30M载波原始幅值
	ch_30M_raw_amplitude = (5.78696 * prog_voltage) / f30_val_bata;
	if (ch_30M_raw_amplitude > 1023)
		ch_30M_raw_amplitude = 1023;
	//! 2M调制信号原始值
	ch_2M_raw_amplitude = (4092 * prog_amj) / f2_val_bata / 100;
	// char buf[10];
	// sprintf(buf,"am%d",ch_2M_raw_amplitude);
	// uart_log_debug(buf);
	//! 计算衰减系数
	amplitudeX = pow(0.635, prog_sd_attenuation * 2);
	//! 写入幅值
	Write_AmplitudeU(0, ch_30M_raw_amplitude);
	Write_AmplitudeU(1, ch_2M_raw_amplitude);
	Write_AmplitudeU(2, ch_30M_raw_amplitude * amplitudeX);
	Write_AmplitudeU(3, ch_2M_raw_amplitude * amplitudeX);
	//! 写入载波频率
	Write_FrequenceU(0, prog_freq * 1000000);
	Write_FrequenceU(2, prog_freq * 1000000);
	Write_FrequenceU(1, am_freq);
	Write_FrequenceU(3, am_freq);
	IO_Update();
	//! Delay 对应ch2,3 （多路径）
	//! AM调制信号
	int delta_phase = prog_sd_delay * 2 * 360 / 1000;
	ch_2M_sm_phase = prog_wireless_sm_phase - delta_phase;
	if (ch_2M_sm_phase < 0)
		ch_2M_sm_phase += 360;
	if (ch_2M_sm_phase > 360)
		ch_2M_sm_phase -= 360;
	Write_PhaseU(3, ch_2M_sm_phase); //? 通道3是AM2M信号
	Write_PhaseU(1, prog_dm_phase);  //! 对于直接路径 直接写入相位
	IO_Update();
	//! 30M 载波
	// TODO 关闭全部两个通道（直通和多径）
	// TODO 打开直通输出
	int ch_sm_phase = 0;
	ch_sm_phase = prog_dm_phase + prog_wireless_sm_phase;
	if (ch_sm_phase < 0)
		ch_sm_phase += 360;
	if (ch_sm_phase > 360)
		ch_sm_phase -= 360;
	uint16_t us = 10 * 1 * prog_sd_delay;
	Write_Phase(0, prog_dm_phase);
	delay_us(us);
	// TODO 打开多径输出
	Write_Phase(2, ch_sm_phase);
}

//TODO TAG MAIN
extern "C" int main(void) {

	HAL_Init(); // 初始化 HAL 库

	SystemClock_Config(); // 配置系统时钟
	DWT_Init();

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
	// uart_log_info("init dac");
	// DAC_Init();
	// initDacOutput();
	// setDacOutput(DAC_CHANNEL_1, 2040 * prog_amj / 100); //! PA4 CH2是PA5
	uart_log_info("init ec11");
	usedEc11Add = ec11Minus;
	usedEc11Minus = ec11Add; //? 反过来直觉一点
	usedEc11Click = ec11Click;
	EC11_Init();
	uart_log_info("init spi1");
	SPI1_Init_PB();
	uart_log_info("init LCD");
	LCD_Init(BLACK);

	// 初始化lcd字符缓冲区
	InitializeBuffer(buffer1, ' ', WHITE, BLACK);
	InitializeBuffer(buffer2, ' ', WHITE, BLACK);

	// // DisplayAlphabet();
	// //  初始化 lcd_print 环境
	// //lcd_print_init(WHITE, BLACK, SCROLL_UP);
	lcd_print_init(WHITE, BLACK, RETURN_TO_ORIGIN);

	//? 绘制屏幕 变量在pos 20开始
	lcd_print("                [Public]        ");
	lcd_print("  Mode              XX");
	lcd_print("  Voltage           100mV");
	lcd_print("  Frequency         30Mhz");
	lcd_print("  AMJ               30%");
	lcd_print("                [Direct]");
	lcd_print("  D_Phase           0Deg");
	lcd_print("               [Multiple]");
	lcd_print("  Delay             50ns");
	lcd_print("  Attenuation       0dB");
	lcd_print("  M_Phase           0Deg");
	lcd_print("  ");
	lcd_print("CUI MODE            XXXXXX");

	initSelectRow(true);
	// uart_log_info("init dma");
	//  lcd_log_info("init dma");
	//  MX_DMA_Init(); // 初始化 DMA
	uart_log_info("init tim8");
	// lcd_log_info("init tim8");
	// MX_TIM8_Init(); // 初始化 TIM8
	// uart_log_info("init pwm output");
	// lcd_log_info("init pwm output");
	// Start_PWM(); //! 初始化PA5上的PWM输出

	uart_log_success("init done");
	uart_print("hello world");

	Init_AD9959();
	progUPDATE();
	while (1) {
		displaySelectRow();
		displayProgInfo();
	}
}

static char SelectedCursor = 'O';

void displaySelectRow() {
	initSelectRow(false);
	static uint16_t fg_color = WHITE;
	static uint16_t bg_color = BLUE;
	switch (select_y) {
	case 0:
		SetChar(1, 0, SelectedCursor, fg_color, bg_color);
		break;
	case 1:
		SetChar(2, 0, SelectedCursor, fg_color, bg_color);
		break;
	case 2:
		SetChar(3, 0, SelectedCursor, fg_color, bg_color);
		break;
	case 3:
		SetChar(4, 0, SelectedCursor, fg_color, bg_color);
		break;
	case 4:
		SetChar(6, 0, SelectedCursor, fg_color, bg_color);
		break;
	case 5:
		SetChar(8, 0, SelectedCursor, fg_color, bg_color);
		break;
	case 6:
		SetChar(9, 0, SelectedCursor, fg_color, bg_color);
		break;
	case 7:
		SetChar(10, 0, SelectedCursor, fg_color, bg_color);
		break;
		// case 8:
		// 	SetChar(10, 0, SelectedCursor, fg_color, bg_color);
		// 	break;
		// case 9:
		// 	SetChar(11, 0, SelectedCursor, fg_color, bg_color);
		// 	break;
		// case 10:
		// 	SetChar(12, 0, SelectedCursor, fg_color, bg_color);
		// 	break;

	default:
		break;
	}
	UpdateScreen();
}

void displayProgInfo() {
	uint16_t fg_color = WHITE;
	uint16_t bg_color = BLACK;
	char buffer[30] = {0};
	auto createString = [](int flag) -> const char* {
		if (flag == AM) {
			return "AM";
		} else {
			return "CW";
		}
	};
	lcd_setString(1, 20, fg_color, bg_color, createString(prog_output_mode));
	strcpy(buffer, "");
	sprintf(buffer, "%dmV", prog_voltage);
	lcd_setString(2, 20, WHITE, BLACK, "           ");
	lcd_setString(2, 20, fg_color, bg_color, buffer);
	strcpy(buffer, "");
	sprintf(buffer, "%dMhz", prog_freq);
	lcd_setString(3, 20, WHITE, BLACK, "           ");
	lcd_setString(3, 20, fg_color, bg_color, buffer);
	strcpy(buffer, "");
	sprintf(buffer, "%d%%", prog_amj);
	if (prog_output_mode == AM) {
		lcd_setString(4, 2, WHITE, BLACK,
		              "AMJ                                    ");
		lcd_setString(4, 20, fg_color, bg_color, buffer);
	} else {
		lcd_setString(4, 2, WHITE, GRAY,
		              "AMJ                                     ");
		lcd_setString(4, 20, fg_color, GRAY, buffer);
	}
	strcpy(buffer, "");
	sprintf(buffer, "%dDeg", prog_dm_phase);
	lcd_setString(6, 20, WHITE, BLACK, "           ");
	lcd_setString(6, 20, fg_color, bg_color, buffer);
	strcpy(buffer, "");
	sprintf(buffer, "%dns", prog_sd_delay);
	lcd_setString(8, 20, WHITE, BLACK, "           ");
	lcd_setString(8, 20, fg_color, bg_color, buffer);
	strcpy(buffer, "");
	sprintf(buffer, "%ddB", prog_sd_attenuation);
	lcd_setString(9, 20, WHITE, BLACK, "           ");
	lcd_setString(9, 20, fg_color, bg_color, buffer);
	strcpy(buffer, "");
	sprintf(buffer, "%dDeg", prog_wireless_sm_phase);
	lcd_setString(10, 20, WHITE, BLACK, "           ");
	lcd_setString(10, 20, fg_color, bg_color, buffer);
	// strcpy(buffer, "");
	// sprintf(buffer, "%dns", prog_wireless_sd_delay);
	// lcd_setString(10, 20, WHITE, BLACK, "           ");
	// lcd_setString(10, 20, fg_color, bg_color, buffer);
	// strcpy(buffer, "");
	// sprintf(buffer, "%ddB", prog_wireless_sd_attenuation);
	// lcd_setString(11, 20, WHITE, BLACK, "           ");
	// lcd_setString(11, 20, fg_color, bg_color, buffer);
	// strcpy(buffer, "");
	// sprintf(buffer, "%d", prog_wireless_sm_phase);
	// lcd_setString(12, 20, WHITE, BLACK, "           ");
	// lcd_setString(12, 20, fg_color, bg_color, buffer);
	strcpy(buffer, "");
	fg_color = BLACK;
	bg_color = WHITE;
	if (flag_edit_mode) {
		bg_color = RED;
		lcd_setString(12, 20, WHITE, BLACK, "EDIT  ");
	} else {
		lcd_setString(12, 20, WHITE, BLACK, "NORMAL");
	}
	switch (select_y) {
	case 0:
		lcd_setString(1, 20, fg_color, bg_color,
		              createString(prog_output_mode));
		break;
	case 1:
		sprintf(buffer, "%dmV", prog_voltage);
		lcd_setString(2, 20, fg_color, bg_color, buffer);
		break;
	case 2:
		sprintf(buffer, "%dMhz", prog_freq);
		lcd_setString(3, 20, fg_color, bg_color, buffer);
		break;
	case 3:
		if (prog_output_mode == AM) {
			sprintf(buffer, "%d%%", prog_amj);
			lcd_setString(4, 20, fg_color, bg_color, buffer);
		} else {
			lcd_setString(4, 20, fg_color, GRAY, buffer);
		}
		break;
	case 4:
		sprintf(buffer, "%dDeg", prog_dm_phase);
		lcd_setString(6, 20, fg_color, bg_color, buffer);
		break;
	case 5:
		sprintf(buffer, "%dns", prog_sd_delay);
		lcd_setString(8, 20, fg_color, bg_color, buffer);
		break;
	case 6:
		sprintf(buffer, "%ddB", prog_sd_attenuation);
		lcd_setString(9, 20, fg_color, bg_color, buffer);
		break;
	case 7:
		sprintf(buffer, "%dDeg", prog_wireless_sm_phase);
		lcd_setString(10, 20, fg_color, bg_color, buffer);
		break;
		// case 8:
		// 	sprintf(buffer, "%dns", prog_wireless_sd_delay);
		// 	lcd_setString(10, 20, fg_color, bg_color, buffer);
		// 	break;
		// case 9:
		// 	sprintf(buffer, "%ddB", prog_wireless_sd_attenuation);
		// 	lcd_setString(11, 20, fg_color, bg_color, buffer);
		// 	break;
		// case 10:
		// 	sprintf(buffer, "%d", prog_wireless_sm_phase);
		// 	lcd_setString(12, 20, fg_color, bg_color, buffer);
		// 	break;
	}
	lcdPrintConfig.cursorY = 208;
	lcdPrintConfig.cursorX = 0;
	lcd_log_debug("debug mode test");
	UpdateScreen();
}

void getNumberChar(uint8_t input_buffer[100], char number_buffer[100]) {
	int j = 0;
	for (int i = 0; i < 100; i++) {
		number_buffer[i] = 0;
	}
	for (int i = 0; i < 100; i++) {
		if (isdigit(input_buffer[i])) {
			number_buffer[j++] = input_buffer[i];
		}
	}
}

void reboot() {
	uart_log_warn("Rebooting MCU...");
	blue_led.switchOn();
	HAL_Delay(1);

	// 执行 MCU 重启操作，可以通过适当的函数实现
	NVIC_SystemReset();
}

void ConvertUint16ToString(uint16_t num, char* str) {
	sprintf(str, "%u", num); // 将 uint16_t 转换为字符串
}
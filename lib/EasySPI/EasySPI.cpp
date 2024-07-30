#include "EasySPI.h"
#include <EasyUART.h>

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/*****************************************************************************
 * @name       :uint8_t SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t Byte)
 * @date       :2023-07-22
 * @function   :Write a byte of data using STM32's hardware SPI
 * @parameters :hspi: SPI handle
                Byte: Data to be written
 * @retvalue   :Data received by the bus
******************************************************************************/
uint8_t SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t Byte) {
	uint8_t receivedByte;
	HAL_SPI_TransmitReceive(hspi, &Byte, &receivedByte, 1, HAL_MAX_DELAY);
	return receivedByte;
}
/*****************************************************************************
 * @name       :void SPI_SetSpeed(SPI_HandleTypeDef* hspi, uint8_t SpeedSet)
 * @date       :2023-07-22
 * @function   :Set hardware SPI Speed
 * @parameters :hspi: SPI handle
                SpeedSet: 0-high speed, 1-low speed
 * @retvalue   :None
******************************************************************************/
void SPI_SetSpeed(SPI_HandleTypeDef* hspi, uint8_t SpeedSet) {
	hspi->Init.BaudRatePrescaler =
	    (SpeedSet == 1) ? SPI_BAUDRATEPRESCALER_2 : SPI_BAUDRATEPRESCALER_32;
	HAL_SPI_Init(hspi);
}
#include "stm32f4xx_hal.h"

// SPI_HandleTypeDef hspi1;

/*****************************************************************************
 * @name       :void SPI1_Init(void)
 * @date       :2023-07-22
 * @function   :Initialize the STM32 hardware SPI1
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void SPI1_Init_PB(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* SPI1 GPIO Configuration
	   PB3     ------> SPI1_SCK
	   PB4     ------> SPI1_MISO
	   PB5     ------> SPI1_MOSI
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1; // Ensure this alternate function
	                                           // is correct for PB5, PB6, PB7
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		// Initialization Error
		Error_Handler();
		uart_log_error("spi1 init fail");
	} else {
		uart_log_success("spi1 init succeed");
	}
	__HAL_SPI_ENABLE(&hspi1);
}

// SPI_HandleTypeDef hspi1;

/*****************************************************************************
 * @name       :void SPI1_Init(void)
 * @date       :2023-07-22
 * @function   :Initialize the STM32 hardware SPI1
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void SPI1_Init_PA(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* SPI1 GPIO Configuration
	   PA5     ------> SPI1_SCK
	   PA6     ------> SPI1_MISO
	   PA7     ------> SPI1_MOSI
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		// Initialization Error
		Error_Handler();
		uart_log_error("spi1 init fail");
	} else {
		uart_log_success("spi1 init succeed");
	}

	__HAL_SPI_ENABLE(&hspi1);
}

// SPI2 Initialization Function
void MX_SPI2_Init(void) {

    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT; // 8 位数据
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; // 确保与原始函数一致
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;     // 确保与原始函数一致
    hspi2.Init.NSS = SPI_NSS_SOFT;             // 软件控制片选
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // 调整预分频器
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

	if (HAL_SPI_Init(&hspi2) != HAL_OK) {
		// Initialization error
		Error_Handler();
		uart_log_error("init spi2 fail");
	} else {
		uart_log_success("init spi2 success");
	}
}

// GPIO Initialization Function
void MX_SPI2_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Enable GPIO clocks
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	// Configure SPI2 SCK (PB13), MISO (PB14), MOSI (PB15)
	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

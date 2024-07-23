#ifndef EASYSPI_H
#define EASYSPI_H

#include "stm32f4xx_hal.h"
#include <ErrorHandler.h>

extern SPI_HandleTypeDef hspi1;


uint8_t SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t Byte);
void SPI_SetSpeed(SPI_HandleTypeDef* hspi, uint8_t SpeedSet);
void SPI1_Init_PB(void);
void SPI1_Init_PA(void);

#endif // EASYSPI_H

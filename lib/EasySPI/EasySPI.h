/*
 * @Author: AndreaFrederica andreafrederica@outlook.com
 * @Date: 2024-07-22 20:21:35
 * @LastEditors: AndreaFrederica andreafrederica@outlook.com
 * @LastEditTime: 2024-07-30 20:30:21
 * @FilePath: \adctest\lib\EasySPI\EasySPI.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef EASYSPI_H
#define EASYSPI_H

#include "stm32f4xx_hal.h"
#include <ErrorHandler.h>

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;


uint8_t SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t Byte);
void SPI_SetSpeed(SPI_HandleTypeDef* hspi, uint8_t SpeedSet);
void SPI1_Init_PB(void);
void SPI1_Init_PA(void);

void MX_SPI2_GPIO_Init(void);
void MX_SPI2_Init(void);

#endif // EASYSPI_H

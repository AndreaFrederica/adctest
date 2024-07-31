/*
 * @Author: AndreaFrederica andreafrederica@outlook.com
 * @Date: 2024-07-30 22:56:37
 * @LastEditors: AndreaFrederica andreafrederica@outlook.com
 * @LastEditTime: 2024-07-31 09:48:27
 * @FilePath: \adctest\lib\AD9959\AD9959.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __AD9959_H
#define __AD9959_H

#include "main.h"

#define CSR_ADD   0x00 // CSR  Channel Select Register(通道选择寄存器)                1 Bytes
#define FR1_ADD   0x01 // FR1  Function Register 1(功能寄存器1)                       3 Bytes
#define FR2_ADD   0x02 // FR2  Function Register 2(功能寄存器2)                       2 Bytes
#define CFR_ADD   0x03 // CFR  Channel Function Register(通道功能寄存器)              3 Bytes
#define CFTW0_ADD 0x04 // CFTW0 Channel Frequency Tuning Word 0(通道频率转换字寄存器)  4 Bytes
#define CPOW0_ADD 0x05 // CPW0 Channel Phase Offset Word 0(通道相位转换字寄存器)      2 Bytes
#define ACR_ADD   0x06 // ACR  Amplitude Control Register(幅度控制寄存器)             3 Bytes
#define LSRR_ADD  0x07 // LSR  Linear Sweep Ramp Rate(通道线性扫描寄存器)             2 Bytes
#define RDW_ADD   0x08 // RDW  LSR Rising Delta Word(通道线性向上扫描寄存器)          4 Bytes
#define FDW_ADD   0x09 // FDW  LSR Falling Delta Word(通道线性向下扫描寄存器)         4 Bytes
#define CW1       0x0A // CW1  Channel Word 1(通道字寄存器1)                         4 Bytes
#define CW2       0x0B // CW2  Channel Word 2(通道字寄存器2)                         4 Bytes
#define CW3       0x0C // CW3  Channel Word 3(通道字寄存器3)                         4 Bytes
#define CW4       0x0D // CW4  Channel Word 4(通道字寄存器4)                         4 Bytes

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint32_t Pin;
} driverIO;

// 定义 GPIOE 引脚
#define SDIO1_Pin GPIO_PIN_8
#define SDIO1_GPIO_Port GPIOD
#define SDIO2_Pin GPIO_PIN_9
#define SDIO2_GPIO_Port GPIOD
#define SDIO3_Pin GPIO_PIN_10
#define SDIO3_GPIO_Port GPIOD
#define PDC_Pin GPIO_PIN_11
#define PDC_GPIO_Port GPIOD
#define RESET_Pin GPIO_PIN_14
#define RESET_GPIO_Port GPIOD
#define SCLK_Pin GPIO_PIN_0
#define SCLK_GPIO_Port GPIOD
#define CS_Pin GPIO_PIN_1
#define CS_GPIO_Port GPIOD
#define UPDATE_Pin GPIO_PIN_2
#define UPDATE_GPIO_Port GPIOD
#define SDIO0_Pin GPIO_PIN_3
#define SDIO0_GPIO_Port GPIOD
#define PS0_Pin GPIO_PIN_4
#define PS0_GPIO_Port GPIOD
#define PS1_Pin GPIO_PIN_5
#define PS1_GPIO_Port GPIOD
#define PS2_Pin GPIO_PIN_6
#define PS2_GPIO_Port GPIOD
#define PS3_Pin GPIO_PIN_7
#define PS3_GPIO_Port GPIOD

extern driverIO SDIO0;
extern driverIO SDIO1;
extern driverIO SDIO2;
extern driverIO SDIO3;
extern driverIO PDC;
extern driverIO RST;
extern driverIO SCLK;
extern driverIO CS;
extern driverIO UPDATE;
extern driverIO PS0;
extern driverIO PS1;
extern driverIO PS2;
extern driverIO PS3;

// AD9959 IO Operation
// IO must be a driverIO struct, VAL must be 0 or 1
#define WRT(IO, VAL) HAL_GPIO_WritePin(IO.GPIOx, IO.Pin, (VAL) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define GET(IO)      HAL_GPIO_ReadPin(IO.GPIOx, IO.Pin)

void Init_AD9959(void);
void delay_9959(uint32_t length);
void InitIO_9959(void);
void InitReset(void);
void IO_Update(void);
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData, uint8_t temp);
void ReadData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData);
void Write_Frequence(uint8_t Channel, uint32_t Freq);
void Write_Amplitude(uint8_t Channel, uint16_t Ampli);
void Write_Phase(uint8_t Channel, uint16_t Phase);
void Write_FrequenceU(uint8_t Channel, uint32_t Freq);
void Write_AmplitudeU(uint8_t Channel, uint16_t Ampli);
void Write_PhaseU(uint8_t Channel, uint16_t Phase);

uint32_t Get_Freq(void);
uint8_t Get_Amp(void);
void Sweep_Frequency(uint8_t Channel, uint32_t Start_Freq, uint32_t Stop_Freq, uint32_t Step, uint32_t time, uint8_t NO_DWELL);
void Channel_Select(uint8_t Channel);
void Stop_AD9959(void);
void AD9959_error(void);
void SET_2FSK(uint8_t Channel, double f_start, double f_stop);
void SET_2ASK(uint8_t Channel, double f, uint16_t A_start, uint16_t A_stop);
void Freq2Word(double f, uint8_t *fWord);
void Amp2Word(uint16_t A, uint8_t *AWord);
#endif

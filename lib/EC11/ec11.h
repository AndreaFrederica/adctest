#ifndef EC11_H
#define EC11_H

#include "stm32f4xx_hal.h"
#include <EasyUART.h>
#include<tools.h>

// 定义EC11引脚
#define EC11_CLK_PIN        GPIO_PIN_1
#define EC11_CLK_GPIO_PORT  GPIOC
#define EC11_DT_PIN         GPIO_PIN_3
#define EC11_DT_GPIO_PORT   GPIOC
#define EC11_SW_PIN         GPIO_PIN_2
#define EC11_SW_GPIO_PORT   GPIOC

// 外部变量
extern int16_t EC11_Dir;     // 旋转方向
extern int16_t EC11_Enter;   // EC11按键

// 外部中断初始化函数声明
void EC11_Init(void);

typedef void (*FunctionType)();
void defaultEc11Add();
void defaultEc11Minus();
void defaultEc11Click();

extern FunctionType usedEc11Add;
extern FunctionType usedEc11Minus;
extern FunctionType usedEc11Click;

#endif // EC11_H



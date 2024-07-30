/*
 * @Author: AndreaFrederica andreafrederica@outlook.com
 * @Date: 2024-07-30 14:42:22
 * @LastEditors: AndreaFrederica andreafrederica@outlook.com
 * @LastEditTime: 2024-07-30 15:47:23
 * @FilePath: \adctest\lib\EC11\ec11.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "ec11.h"

// 定义外部变量
int16_t EC11_Dir = 0;      // 旋转方向
int16_t EC11_Enter = 0;    // EC11按键
FunctionType usedEc11Add = defaultEc11Add;
FunctionType usedEc11Minus = defaultEc11Minus;
FunctionType usedEc11Click = defaultEc11Click;

void defaultEc11Add(){}
void defaultEc11Minus(){}
void defaultEc11Click(){}

// EXTI0_IRQn中断服务程序里处理边沿检测
extern "C" void EXTI1_IRQHandler(void)
{
    delay_us(1000); // 很重要*****
    if(__HAL_GPIO_EXTI_GET_IT(EC11_CLK_PIN) != RESET)
    {
        if(HAL_GPIO_ReadPin(EC11_CLK_GPIO_PORT, EC11_CLK_PIN) == GPIO_PIN_SET)
        {
            if(HAL_GPIO_ReadPin(EC11_DT_GPIO_PORT, EC11_DT_PIN) == GPIO_PIN_SET)
            {
                EC11_Dir = 1;
                //HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2); // 蓝灯闪
                //uart_log_debug("EC11_Dir: +1"); // 顺时针
                usedEc11Add();
            }
            else
            {
                EC11_Dir = -1;
                //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_5); // 红灯闪
                //uart_log_debug("EC11_Dir: -1"); // 逆时针
                usedEc11Minus();
            }
        }
        __HAL_GPIO_EXTI_CLEAR_IT(EC11_CLK_PIN); // 清中断
    }
}

// EXTI2_IRQn中断服务程序里处理EC11按键
extern "C" void EXTI2_IRQHandler(void)
{
    delay_us(1000); // 很重要*****
    if(__HAL_GPIO_EXTI_GET_IT(EC11_SW_PIN) != RESET) // 判断EC11按键状态
    {
        if(HAL_GPIO_ReadPin(EC11_SW_GPIO_PORT, EC11_SW_PIN) == GPIO_PIN_RESET)
            EC11_Enter = 10; // 赋值非零
        else
            EC11_Enter = 0; // 赋值零

        if(EC11_Enter != 0)
            //uart_log_debug("EC11_Enter!"); // 按下确认键
            usedEc11Click();

        __HAL_GPIO_EXTI_CLEAR_IT(EC11_SW_PIN); // 清中断
    }
}

// 外部中断初始化程序，初始化PC0,PC2为中断输入
void EC11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOC时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    // 使能SYSCFG时钟
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    // 配置EC11-sw引脚: PC2, 闲时上拉，按下置低电平
    GPIO_InitStruct.Pin = EC11_SW_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(EC11_SW_GPIO_PORT, &GPIO_InitStruct);

    // 配置EC11-CLK引脚: PC0, 上升沿触发
    GPIO_InitStruct.Pin = EC11_CLK_PIN | EC11_DT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(EC11_CLK_GPIO_PORT, &GPIO_InitStruct);

    // 配置NVIC
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

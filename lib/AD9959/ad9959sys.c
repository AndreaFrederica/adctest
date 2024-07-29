/**********************************************************
 * @ File name -> sys.c
 * @ Version   -> V1.0
 * @ Date      -> 12-26-2013
 * @ Brief     -> 系统设置相关的函数
 **********************************************************/

#include "ad9959sys.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

/**********************************************************
 * 函数功能 ---> 读取芯片闪存容量
 * 入口参数 ---> *STMCapa：容量字符显示缓存
 * 返回数值 ---> 容量（十进制）
 * 功能说明 ---> none
 **********************************************************/
void STM32_Flash_Capacity(uint8_t* STMCapa) {
    uint16_t capa;

    capa = *((uint16_t*)0x1FFF7A22); // 读取闪存容量寄存器，低16位有效

    capa = ((capa >> 12) * 4096 + ((capa >> 8) & 0x0F) * 256 +
            ((capa >> 4) & 0x0F) * 16 + (capa & 0x0F)); // 转换成十进制

    STMCapa[0] = 'M';
    STMCapa[1] = 'C';
    STMCapa[2] = 'U';
    STMCapa[3] = 'C';
    STMCapa[4] = 'a';
    STMCapa[5] = 'p';
    STMCapa[6] = ':';

    if ((capa / 1000) != 0)
        STMCapa[7] = capa / 1000 + 48; // 千位不为0时显示
    else
        STMCapa[7] = ' ';

    STMCapa[8] = capa % 1000 / 100 + 48; // 百位
    STMCapa[9] = capa % 100 / 10 + 48;   // 十位
    STMCapa[10] = capa % 10 + 48;        // 个位
    STMCapa[11] = 'K';
    STMCapa[12] = 'b';
}

/**********************************************************
 * 函数功能 ---> 读取CPUID
 * 入口参数 ---> none
 * 返回数值 ---> CPUID（十六进制）
 * 功能说明 ---> none
 **********************************************************/
void STM32_CPUID(uint8_t* IDbuff) {
    uint32_t CPUID;
    CPUID = *((uint32_t*)0xE000ED00);
    sprintf((char*)IDbuff, "CPU ID:%08X", CPUID);
}

/**********************************************************
 * 函数功能 ---> 设置向量表偏移地址
 * 入口参数 ---> NVIC_VectTab：基址
 *              Offset：偏移量
 * 返回数值 ---> none
 * 功能说明 ---> none
 **********************************************************/
void MY_NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset) {
    SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

/**********************************************************
 * 函数功能 ---> 设置中断分组
 * 入口参数 ---> NVIC_PriorityGroup: 中断分组
 * 返回数值 ---> none
 * 功能说明 ---> 0 ~ 4组，共计有5组
 **********************************************************/
void MY_NVIC_PriorityGroup_Config(uint32_t NVIC_PriorityGroup) {
    HAL_NVIC_SetPriorityGrouping(NVIC_PriorityGroup);
}

/**********************************************************
 * 函数功能 ---> 设置中断分组优先级
 * 入口参数 ---> NVIC_PreemptionPriority：抢先优先级
 *               NVIC_Subpriority：响应优先级
 *               NVIC_Channel：中断编号
 *               NVIC_Group: 中断分组
 * 返回数值 ---> none
 * 功能说明 ---> none
 **********************************************************/
void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,
                  uint8_t NVIC_Subpriority,
                  uint8_t NVIC_Channel,
                  uint32_t NVIC_Group) {
    MY_NVIC_PriorityGroup_Config(NVIC_Group); // 设置中断分组

    HAL_NVIC_SetPriority((IRQn_Type)NVIC_Channel, NVIC_PreemptionPriority, NVIC_Subpriority); // 设置优先级
    HAL_NVIC_EnableIRQ((IRQn_Type)NVIC_Channel); // 使能中断
}

/**********************************************************
 * 函数功能 ---> THUMB指令不支持汇编内联
 * 入口参数 ---> none
 * 返回数值 ---> none
 * 功能说明 ---> 采用如下方法实现执行汇编指令WFI
 **********************************************************/
// __asm void WFI_SET(void) { WFI; }
void WFI_SET(void) {
    __WFI();
}

/**********************************************************
 * 函数功能 ---> 所有时钟寄存器复位
 * 入口参数 ---> none
 * 返回数值 ---> none
 * 功能说明 ---> 不能在这里执行所有外设复位!否则至少引起串口不工作
 **********************************************************/
void MY_RCC_DeInit(void) {
    HAL_RCC_DeInit();
    
    // 配置向量表
#ifdef VECT_TAB_RAM
    SCB->VTOR = SRAM_BASE | 0x0;
#else
    SCB->VTOR = FLASH_BASE | 0x0;
#endif
}

/**********************************************************
 * 函数功能 ---> 设置芯片进入待机模式
 * 入口参数 ---> none
 * 返回数值 ---> none
 * 功能说明 ---> 利用WKUP引脚唤醒（PA.0）
 **********************************************************/
void SYS_Standby(void) {
    HAL_PWR_EnterSTANDBYMode();
}

/**********************************************************
 * 函数功能 ---> 系统软复位
 * 入口参数 ---> none
 * 返回数值 ---> none
 * 功能说明 ---> none
 **********************************************************/
void SYS_SoftReset(void) {
    HAL_NVIC_SystemReset();
}

/**********************************************************
 * 函数功能 ---> JTAG模式设置
 * 入口参数 ---> mode：模式参数
 *                    000：JTAG-DP + SW-DP（复位状态）
 *                    001：JTAG-DP + SW-DP（除了JNTRST引脚，释放JRST引脚）
 *                    010：JTAG-DP接口禁止，SW-DP接口允许
 *                    100：JTAG-DP接口和SW-DP接口都禁止
 *                    xxx：其他值，禁止
 * 返回数值 ---> none
 * 功能说明 ---> none
 **********************************************************/
void STM_JTAG_Set(uint32_t mode) {
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_DISABLE(); // 禁止JTAG-DP和SW-DP接口

    if (mode == 0x00000001) {
        __HAL_AFIO_REMAP_SWJ_NONJTRST();
    } else if (mode == 0x00000002) {
        __HAL_AFIO_REMAP_SWJ_NOJTAG();
    } else if (mode == 0x00000004) {
        __HAL_AFIO_REMAP_SWJ_DISABLE();
    }
}

/**********************************************************
 * 函数功能 ---> 系统时钟初始化
 * 入口参数 ---> pll：倍频数。取值范围：2 ~ 16
 * 返回数值 ---> none
 * 功能说明 ---> none
 **********************************************************/
void STM_Clock_Init(uint8_t pll) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    MY_RCC_DeInit(); // 复位并配置向量表，并且将外部中断和外设时钟全部关闭

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8; // 设置HSE预分频因子
    RCC_OscInitStruct.PLL.PLLN = pll; // 设置主倍频因子
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // 设置主系统时钟分频因子
    RCC_OscInitStruct.PLL.PLLQ = 7; // 设置USB时钟分频因子
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**********************************************************
 * 函数功能 ---> BCD码转为HEX
 * 入口参数 ---> BCD_Data：要转换的BCD数据
 * 返回数值 ---> HEX码
 * 功能说明 ---> none
 **********************************************************/
uint16_t BCD_to_HEX(uint16_t BCD_Data) {
    return ((BCD_Data >> 12) * 1000 + ((BCD_Data >> 8) & 0x0F) * 100 + ((BCD_Data >> 4) & 0x0F) * 10 + (BCD_Data & 0x0F));
}

/**********************************************************
 * 函数功能 ---> HEX码转为BCD
 * 入口参数 ---> HEX_Data：要转换的HEX数据
 * 返回数值 ---> BCD码
 * 功能说明 ---> none
 **********************************************************/
uint16_t HEX_to_BCD(uint16_t HEX_Data) {
    return (((HEX_Data / 1000) << 12) | (((HEX_Data % 1000) / 100) << 8) | (((HEX_Data % 100) / 10) << 4) | (HEX_Data % 10));
}

/**********************************************************
 * 函数功能 ---> 16进制码转为10进制
 * 入口参数 ---> HX_Data：要转换的16进制数据
 * 返回数值 ---> 10进制
 * 功能说明 ---> none
 **********************************************************/
uint16_t HX_to_DX(uint16_t HX_Data) {
    return (((HX_Data >> 12) * 1000) + ((HX_Data >> 8 & 0x000F) * 100) + ((HX_Data >> 4 & 0x000F) * 10) + (HX_Data & 0x000F));
}

/**********************************************************
 * 函数功能 ---> HEX转BCD（32位数）
 * 入口参数 ---> hex：要转换的HEX数据
 *               *bcd：转换的BCD数据
 * 返回数值 ---> none
 * 功能说明 ---> none
 **********************************************************/
void HEX_to_BCD32(uint32_t hex, uint8_t* bcd) {
    for (int i = 0; i < 8; i++) {
        bcd[i] = (hex % 10);
        hex /= 10;
    }
}

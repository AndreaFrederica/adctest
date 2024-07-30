/**********************************************************
 * @ File name -> sys.h
 * @ Version   -> V1.0
 * @ Date      -> 12-26-2013
 * @ Brief     -> 系统设置相关的函数头文件
 **********************************************************/

#ifndef _ad_sys_h_
#define _ad_sys_h_

/**********************************************************
                     外部函数头文件
**********************************************************/

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "string.h"

// #include "alloc.h"

/**********************************************************
                      动态数据表管理
**********************************************************

#define MaxSize					100	//设置最大的连续空间

typedef uint16_t	ElemType;	//定义数据类型

typedef struct
{
    ElemType *elem;		//顺序表的首地址
    uint16_t length;	//顺序表的长度（表中元素的个数）
    uint16_t listsize;
//顺序表占用内存空间的大小（以sizeof(ElemType)为单位，由MaxSize规定） }Sqlist;
                                                                     */
/**********************************************************
                    JTAG模式设置定义
**********************************************************/

#define JTAG_SWD_Enable 0x00000000  // 复位值
#define JNTRST_Disable 0x00000001   // JNTRST引脚释放
#define SWD_Enable 0x00000010       // JTAG关闭，SWD开启
#define JTAG_SWD_Disable 0x00000100 // JTAG和SWD都关闭

/**********************************************************
                    位带操作相关宏定义
              参考《CM3权威指南》第87 ~ 92页
**********************************************************/

// 0,不支持ucos
// 1,支持ucos
#define SYSTEM_SUPPORT_OS 0 // 定义系统文件夹是否支持UCOS

// 位带操作,实现51类似的GPIO控制功能
// 具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
// IO口操作宏定义
#define BITBAND(addr, bitnum) \
	((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long*)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
// IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 20) // 0x40020014
#define GPIOB_ODR_Addr (GPIOB_BASE + 20) // 0x40020414
#define GPIOC_ODR_Addr (GPIOC_BASE + 20) // 0x40020814
#define GPIOD_ODR_Addr (GPIOD_BASE + 20) // 0x40020C14
#define GPIOE_ODR_Addr (GPIOE_BASE + 20) // 0x40021014
#define GPIOF_ODR_Addr (GPIOF_BASE + 20) // 0x40021414
#define GPIOG_ODR_Addr (GPIOG_BASE + 20) // 0x40021814
#define GPIOH_ODR_Addr (GPIOH_BASE + 20) // 0x40021C14
#define GPIOI_ODR_Addr (GPIOI_BASE + 20) // 0x40022014

#define GPIOA_IDR_Addr (GPIOA_BASE + 16) // 0x40020010
#define GPIOB_IDR_Addr (GPIOB_BASE + 16) // 0x40020410
#define GPIOC_IDR_Addr (GPIOC_BASE + 16) // 0x40020810
#define GPIOD_IDR_Addr (GPIOD_BASE + 16) // 0x40020C10
#define GPIOE_IDR_Addr (GPIOE_BASE + 16) // 0x40021010
#define GPIOF_IDR_Addr (GPIOF_BASE + 16) // 0x40021410
#define GPIOG_IDR_Addr (GPIOG_BASE + 16) // 0x40021810
#define GPIOH_IDR_Addr (GPIOH_BASE + 16) // 0x40021C10
#define GPIOI_IDR_Addr (GPIOI_BASE + 16) // 0x40022010

// IO口操作,只对单一的IO口!
// 确保n的值小于16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // 输出
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // 输出
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  // 输入

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // 输出
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  // 输入

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // 输出
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  // 输入

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // 输出
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  // 输入

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // 输出
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  // 输入

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // 输出
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  // 输入

#define PHout(n) BIT_ADDR(GPIOH_ODR_Addr, n) // 输出
#define PHin(n) BIT_ADDR(GPIOH_IDR_Addr, n)  // 输入

#define PIout(n) BIT_ADDR(GPIOI_ODR_Addr, n) // 输出
#define PIin(n) BIT_ADDR(GPIOI_IDR_Addr, n)  // 输入

/**********************************************************
                     外部调用功能函数
**********************************************************/

void STM32_Flash_Capacity(uint8_t* STMCapa); // 读取芯片闪存容量

void STM32_CPUID(uint8_t* IDbuff); // 读取CPUID

void STM_Clock_Init(uint8_t pll); // 系统时钟初始化

void MY_NVIC_SetVectorTable(uint32_t NVIC_VectTab,
                            uint32_t Offset); // 设置向量表偏移地址

void MY_NVIC_PriorityGroup_Config(uint32_t NVIC_PriorityGroup); // 设置中断分组

void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,
                  uint8_t NVIC_Subpriority,
                  uint8_t NVIC_Channel,
                  uint32_t NVIC_Group);

void MY_RCC_DeInit(void); // 所有时钟寄存器复位

void SYS_Standby(void); // 设置芯片进入待机模式

void SYS_SoftReset(void); // 系统软复位

void STM_JTAG_Set(uint32_t mode); // JTAG模式设置

uint8_t BCD_to_HEX(uint8_t BCD_Data); // BCD码转为HEX

uint8_t HEX_to_BCD(uint8_t HEX_Data); // HEX码转为BCD

uint16_t DX_to_HX(uint16_t DX_Data); // 10进制码转为16进制

uint16_t HX_to_DX(uint16_t HX_Data); // 16进制码转为10进制

// void Sqlist_Init(Sqlist *LIST);	//初始化数据列表
// void Sqlist_DeInit(void);	//复位数据列表
// void InsertElem(Sqlist *L,uint16_t i,ElemType item);
// //向一个动态的数据列表插入一个元素

#endif

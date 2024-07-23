/*
 * @Author: AndreaFrederica andreafrederica@outlook.com
 * @Date: 2024-07-22 19:51:14
 * @LastEditors: AndreaFrederica andreafrederica@outlook.com
 * @LastEditTime: 2024-07-22 21:36:07
 * @FilePath: \Demo_STM32F407ZGT6_Hardware_SPI\SYSTEM\sys\sys.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "f407sys.h"

//********************************************************************************
// 修改说明
// 无
//////////////////////////////////////////////////////////////////////////////////

// THUMB指令不支持汇编内联
// 采用如下方法实现执行汇编指令WFI
// __asm void WFI_SET(void) { WFI; }
// // 关闭所有中断(但是不包括fault和NMI中断)
// __asm void INTX_DISABLE(void) { CPSID I BX LR }
// // 开启所有中断
// __asm void INTX_ENABLE(void) { CPSIE I BX LR }
// // 设置栈顶地址
// // addr:栈顶地址
// __asm void MSR_MSP(u32 addr) {
//   MSR MSP, r0 // set Main Stack value
//                BX r14
// }

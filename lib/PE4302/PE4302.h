/*
 * @Author: AndreaFrederica andreafrederica@outlook.com
 * @Date: 2024-07-31 18:02:18
 * @LastEditors: AndreaFrederica andreafrederica@outlook.com
 * @LastEditTime: 2024-07-31 18:29:03
 * @FilePath: \adctest\lib\PE4302\PE4302.H
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _PE4302_H_
#define _PE4302_H_

#include "tools.h"
#include "f407sys.h"


#define PE_LE_0 PCout(9)
#define PE_CLK PCout(10)
#define PE_DAT PCout(11)

#define PE_LE_0_EN PE_LE_0 = 1
#define PE_LE_0_DIS PE_LE_0 = 0

#define PE_CLK_0 PE_CLK = 0
#define PE_CLK_1 PE_CLK = 1

#define PE_DAT_0 PE_DAT = 0
#define PE_DAT_1 PE_DAT = 1

void PE_GPIO_Init(void);
void PE4302_0_Set(unsigned char db);
void PE4302_1_Set(unsigned char db);
#endif

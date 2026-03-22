#ifndef __KEYIN_H
#define __KEYIN_H
#include "sys.h"

#define KEYGAIN GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define KEY0 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)
#define KEY1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)
#define KEY2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)

#define KEY0_PRES 1    // KEY0=0
#define KEY1_PRES 2    // KEY1=0
#define KEY2_PRES 3    // KEY2=0
#define KEYGAIN_PRES 4 // KEYGAIN=1
void KEYIN_Init(void); // 按键初始化
/*按键扫描函数*/
u8 KEY_Scan(u8); // u8 unsigned int 8 无符号8位整型数据
#endif

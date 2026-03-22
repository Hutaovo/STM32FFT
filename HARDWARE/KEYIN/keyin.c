/**
 * @file    keyin.c
 * @brief   按键输入模块
 * @details 配置按键GPIO并实现按键扫描功能
 *          - KEY0: PA1 (采样率显示切换)
 *          - KEY1: PA2 (减小采样率)
 *          - KEY2: PA3 (增大采样率)
 *          - KEYGAIN: PA0 (增益切换)
 * @author  用户
 * @version 1.0
 * @date    2024
 */

#include "keyin.h"
#include "delay.h"

/**
 * @brief   按键GPIO初始化函数
 * @details 配置PA0-PA3为上拉输入模式
 *          按键按下时引脚接地，读取到低电平
 * @param   无
 * @return  无
 */
void KEYIN_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief   按键扫描函数
 * @details 检测按键按下状态，支持连续/不连续按键模式
 *          
 *          按键定义:
 *          - KEY0_PRES:  KEY0按下
 *          - KEY1_PRES:  KEY1按下
 *          - KEY2_PRES:  KEY2按下
 *          - KEYGAIN_PRES: KEYGAIN按下
 * 
 * @param   mode 按键模式
 *              - 0: 不支持连续按(按下一次只返回一次)
 *              - 1: 支持连续按(按住不放持续返回)
 * @return  按键值
 *              - 0: 无按键按下
 *              - KEY0_PRES~KEYGAIN_PRES: 对应按键按下
 */
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;

    if (mode)
        key_up = 1;

    if (key_up && (KEY0 == 0 || KEY1 == 0 || KEYGAIN == 0))
    {
        delay_ms(10);
        key_up = 0;

        if (KEY0 == 0)
            return KEY0_PRES;
        else if (KEY1 == 0)
            return KEY1_PRES;
        else if (KEY2 == 0)
            return KEY2_PRES;
        else if (KEYGAIN == 0)
            return KEYGAIN_PRES;
    }
    else if ((KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEYGAIN == 1))
    {
        key_up = 1;
    }

    return 0;
}

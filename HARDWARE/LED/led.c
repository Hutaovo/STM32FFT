/**
 * @file    led.c
 * @brief   LED指示灯模块
 * @details 配置LED GPIO用于状态指示
 *          - LED0: PB0 (采样率显示切换指示)
 *          - LED1: PD2 (增益切换指示)
 * @author  用户
 * @version 1.0
 * @date    2024
 */

#include "led.h"

/**
 * @brief   LED GPIO初始化函数
 * @details 配置PB0和PD2为推挽输出，初始状态为熄灭(高电平)
 * @param   无
 * @return  无
 */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
}

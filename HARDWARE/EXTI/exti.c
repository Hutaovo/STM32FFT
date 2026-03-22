/**
 * @file    exti.c
 * @brief   外部中断模块
 * @details 配置按键外部中断，用于响应按键操作
 *          - EXTI1: KEY0 (PA1) - 切换采样率显示
 *          - EXTI2: KEY1 (PA2) - 减小采样率
 *          - EXTI3: KEY2 (PA3) - 增大采样率
 * @author  用户
 * @version 1.0
 * @date    2024
 */

#include "exti.h"
#include "keyin.h"

/**
 * @brief   外部中断初始化函数
 * @details 配置PA1/PA2/PA3的外部中断线
 *          - 下降沿触发(按键按下)
 *          - 抢占优先级2，响应优先级1
 * @param   无
 * @return  无
 */
void EXTIX_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct; // 外部中断结构体
    NVIC_InitTypeDef NVIC_InitStruct; // NVIC结构体

    KEYIN_Init(); // 初始化按键
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // 使能AFIO时钟

    /* EXTI1配置 - KEY0 (PA1) */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); // 配置PA1为外部中断线  
    EXTI_InitStruct.EXTI_Line = EXTI_Line1; // 配置EXTI1为PA1
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // 使能EXTI1
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // 配置为中断模式
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_Init(&EXTI_InitStruct); // 初始化EXTI1

    NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn; // 配置EXTI1_IRQn中断
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // 使能EXTI1_IRQn中断
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级2
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // 响应优先级1
    NVIC_Init(&NVIC_InitStruct); // 初始化NVIC

    /* EXTI2配置 - KEY1 (PA2) */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2); // 配置PA2为外部中断线
    EXTI_InitStruct.EXTI_Line = EXTI_Line2; // 配置EXTI2为PA2
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // 使能EXTI2
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // 配置为中断模式
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_Init(&EXTI_InitStruct); // 初始化EXTI2

    NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn; // 配置EXTI2_IRQn中断
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // 使能EXTI2_IRQn中断
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级2
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // 响应优先级1
    NVIC_Init(&NVIC_InitStruct); // 初始化NVIC

    /* EXTI3配置 - KEY2 (PA3) */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3); // 配置PA3为外部中断线
    EXTI_InitStruct.EXTI_Line = EXTI_Line3; // 配置EXTI3为PA3
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // 使能EXTI3
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // 配置为中断模式
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_Init(&EXTI_InitStruct); // 初始化EXTI3

    NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn; // 配置EXTI3_IRQn中断
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // 使能EXTI3_IRQn中断
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级2
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // 响应优先级1
    NVIC_Init(&NVIC_InitStruct); // 初始化NVIC
   }

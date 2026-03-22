/**
 * @file    adc.h
 * @brief   ADC采样模块头文件
 * @details 声明ADC初始化及相关函数
 * @author  用户
 * @version 1.0
 * @date    2024
 */

#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

/**
 * @brief   ADC初始化
 */
void Adc_Init(void);

/**
 * @brief   DAC模式初始化(预留)
 */
void DAC_Mode_Init(void);

/**
 * @brief   TIM2 PWM初始化
 * @param   arr 自动重装载值
 * @param   psc 预分频值
 */
void TIM2_PWM_Init(u16 arr, u16 psc);

/**
 * @brief   ADC DMA配置
 * @param   DMA_CHx DMA通道
 * @param   cpar    外设地址
 * @param   cmar    内存地址
 * @param   cndtr   数据数量
 */
void ADC_DMA1_Config(DMA_Channel_TypeDef *DMA_CHx, u32 cpar, u32 cmar, u16 cndtr);

/**
 * @brief   获取ADC值
 * @param   ch ADC通道
 * @return  ADC采样值
 */
u16 Get_Adcval(u8 ch);

/**
 * @brief   获取ADC平均值
 * @param   ch    ADC通道
 * @param   times 采样次数
 * @return  平均ADC值
 */
u16 Get_Adc_Adverage(u8 ch, u8 times);

#endif

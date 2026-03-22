/**
 * @file    lcd_init.h
 * @brief   ST7789 LCD驱动初始化头文件
 * @details 定义LCD引脚宏和函数声明
 *          使用硬件SPI2+DMA接口:
 *          - SCK:  PB13 (SPI2_SCK)
 *          - MOSI: PB15 (SPI2_MOSI)
 *          - CS:   PB12 (软件控制)
 *          - DC:   PA8  (数据/命令选择)
 *          - RES:  PA9  (复位)
 *          - BLK:  PA10 (背光)
 *          - DMA:  DMA1通道5 (SPI2_TX)
 * @author  用户
 * @version 3.0
 * @date    2024
 */

#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "sys.h"

#define USE_HORIZONTAL 3    /* 显示方向: 0或1为竖屏, 2或3为横屏 */

#define LCD_W 240           /* LCD宽度 */
#define LCD_H 240           /* LCD高度 */

/* LCD引脚控制宏定义 - 硬件SPI2 */

#define LCD_CS_Clr()  GPIO_ResetBits(GPIOB, GPIO_Pin_12)   /* CS片选拉低(选中) */
#define LCD_CS_Set()  GPIO_SetBits(GPIOB, GPIO_Pin_12)     /* CS片选拉高(取消选中) */

#define LCD_DC_Clr()  GPIO_ResetBits(GPIOA, GPIO_Pin_8)    /* DC数据/命令选择拉低(命令) */
#define LCD_DC_Set()  GPIO_SetBits(GPIOA, GPIO_Pin_8)      /* DC数据/命令选择拉高(数据) */

#define LCD_RES_Clr() GPIO_ResetBits(GPIOA, GPIO_Pin_9)    /* RES复位线拉低 */
#define LCD_RES_Set() GPIO_SetBits(GPIOA, GPIO_Pin_9)      /* RES复位线拉高 */

#define LCD_BLK_Clr() GPIO_ResetBits(GPIOA, GPIO_Pin_10)   /* BLK背光控制拉低(关闭) */
#define LCD_BLK_Set() GPIO_SetBits(GPIOA, GPIO_Pin_10)     /* BLK背光控制拉高(打开) */

/* 函数声明 */

/**
 * @brief   LCD SPI2+DMA初始化
 */
void LCD_SPI_DMA_Init(void);

/**
 * @brief   硬件SPI写入8位数据(阻塞方式)
 * @param   dat 8位数据
 */
void LCD_SPI_WriteByte(u8 dat);

/**
 * @brief   DMA方式发送数据
 * @param   data 数据指针
 * @param   size 数据大小(字节)
 */
void LCD_SPI_DMA_Send(u8 *data, u16 size);

/**
 * @brief   写入8位数据
 * @param   dat 8位数据
 */
void LCD_WR_DATA8(u8 dat);

/**
 * @brief   写入16位数据
 * @param   dat 16位数据
 */
void LCD_WR_DATA(u16 dat);

/**
 * @brief   写入命令
 * @param   dat 命令字节
 */
void LCD_WR_REG(u8 dat);

/**
 * @brief   设置显示区域地址
 * @param   x1 起始列
 * @param   y1 起始行
 * @param   x2 结束列
 * @param   y2 结束行
 */
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2);

/**
 * @brief   LCD初始化
 */
void LCD_Init(void);

#endif

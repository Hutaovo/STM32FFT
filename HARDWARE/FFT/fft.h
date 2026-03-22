/**
 * @file    fft.h
 * @brief   快速傅里叶变换(FFT)头文件
 * @details 定义FFT相关数据结构、常量和函数声明
 * @author  用户
 * @version 1.0
 * @date    2024
 */

#ifndef FFT_H
#define FFT_H

#define FFT_N 512        /* FFT变换点数，必须为2的幂次 */
#define PI2 6.283185     /* 2π的近似值，用于三角函数计算 */

/**
 * @brief 复数结构体
 */
struct compx
{
    float real;          /* 实部 */
    float imag;          /* 虚部 */
};

/* 全局变量声明 */
extern struct compx Compx[];              /* FFT输入/输出数组 */
extern float SIN_TAB[];                   /* 正弦查表 */

/* 函数声明 */
extern void create_sin_tab(float *sin_t); /* 创建正弦查表 */
extern void FFT(struct compx *xin);       /* 快速傅里叶变换 */
extern void Get_Result(struct compx *xin);/* 计算FFT结果模值 */
extern void Hanning_Window(struct compx *xin, int n);  /* 汉宁窗函数 */
extern void Remove_DC(struct compx *xin, int n);       /* 去除直流分量 */

#endif

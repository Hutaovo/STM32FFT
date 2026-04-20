/**
 * @file    fft.c
 * @brief   快速傅里叶变换(FFT)实现
 * @details 实现512点FFT算法，包括:
 *          - 正弦/余弦查表法加速计算
 *          - 基2时间抽取FFT算法(蝶形运算)
 *          - 汉宁窗函数减少频谱泄漏
 *          - 直流分量去除
 * @author  用户
 * @version 1.0
 * @date    2024
 */

#include "math.h"
#include "fft.h"

struct compx Compx[FFT_N] = {0}; /* FFT输入/输出复数数组 */
float SIN_TAB[FFT_N / 4 + 1];	 /* 正弦查表，存储1/4周期的正弦值 */

/**
 * @brief   创建正弦采样表
 * @details 预先计算并存储FFT_N/4+1个正弦值，用于后续查表计算
 *          利用正弦函数的对称性，只需存储1/4周期即可
 * @param   sin_t 正弦表数组指针
 * @return  无
 */
void create_sin_tab(float *sin_t)
{
	int i;
	for (i = 0; i <= FFT_N / 4; i++) // 遍历FFT_N/4+1个正弦值
	{
		sin_t[i] = sin(PI2 * i / FFT_N); // 计算正弦值，存储到sin_t[i]中
	}
}

/**
 * @brief   查表法计算正弦值
 * @details 根据对称性，从1/4周期正弦表获取任意位置的正弦值
 *          sin(x)在0~π范围内对称，利用此特性减少存储空间
 * @param   n 采样点索引(0 ~ FFT_N/2)
 * @return  对应位置的正弦值
 */
float sin_tab(int n)
{
	if (n > FFT_N / 4) 
	{
		n = FFT_N / 2 - n; // 对称性关系，将n映射到0~FFT_N/4-1范围内
	}
	return SIN_TAB[n]; // 返回对应位置的正弦值
}

/**
 * @brief   查表法计算余弦值
 * @details 利用cos(x) = sin(π/2 - x)的关系，从正弦表获取余弦值
 * @param   n 采样点索引(0 ~ FFT_N/2)
 * @return  对应位置的余弦值
 */
float cos_tab(int n)
{
	if (n < FFT_N / 4) 
	{
		return SIN_TAB[FFT_N / 4 - n]; // 对称性关系，将n映射到0~FFT_N/4-1范围内
	}
	else
	{
		return -SIN_TAB[n - FFT_N / 4]; // 对称性关系，将n映射到0~FFT_N/4-1范围内
	}
}

/**
 * @brief   快速傅里叶变换(FFT)
 * @details 对输入的复数数组进行基2时间抽取FFT变换
 *          算法步骤:
 *          1. 位反转排序(雷德算法) - 将自然顺序转换为倒位序
 *          2. 蝶形运算 - log2(N)级运算，每级包含N/2个蝶形
 *
 * @param   xin 输入复数数组，变换结果也存于此数组
 * @return  无(结果存于xin中)
 *
 * @note    输入: xin[0..N-1].real为时域采样值，xin[].imag=0
 *          输出: xin[0..N-1]为频域复数值
 *                xin[0]为直流分量
 *                xin[k]对应频率 f = k * fs/N (k=1,2,...,N/2-1)
 */
void FFT(struct compx *xin)
{
	register int f, m, nv2, nm1, i, k, l, j, u = 0; // 注册变量，提高效率
	struct compx w, t; // 旋转因子和临时变量

	/* 位反转排序 - 雷德算法 */
	nv2 = FFT_N / 2; // FFT_N/2，用于位反转排序
	nm1 = FFT_N - 1; // FFT_N-1，用于位反转排序
	for (i = 0; i < nm1; ++i) 
	{
		if (i < j)
		{
			t = xin[j]; // 临时变量，用于交换xin[j]和xin[i]
			xin[j] = xin[i]; // 交换xin[j]和xin[i]
			xin[i] = t; // 交换xin[j]和xin[i]
		}
		k = nv2; // 临时变量，用于位反转排序
		while (k <= j) 
		{
			j = j - k; // 位反转排序，将j转换为倒位序
			k = k / 2; // 临时变量，用于位反转排序
		}
		j = j + k; // 位反转排序，将j转换为自然顺序
	}

	/* 蝶形运算 */
	{
		int le, lei, ip; // 临时变量，用于蝶形运算
		f = FFT_N; // FFT_N，用于蝶形运算
		for (l = 1; (f = f >> 1) != 1; ++l)
			;

		/* l级蝶形运算，l = log2(N) */
		for (m = 1; m <= l; m++)
		{
			le = 1 << m; // 临时变量，用于蝶形运算
			lei = le >> 1; // 临时变量，用于蝶形运算

			/* 计算不同旋转因子的蝶形 */
			for (j = 0; j < lei; j++)
			{
				u = j * FFT_N / le; // 计算旋转因子的索引
				w.real = cos_tab(u); // 查表法计算余弦值
				w.imag = -sin_tab(u); // 查表法计算正弦值

				/* 相同旋转因子的蝶形运算 */
				for (i = j; i < FFT_N; i = i + le)
				{
					ip = i + lei;

					/* 复数乘法: t = xin[ip] * w */
					t.real = xin[ip].real * w.real - xin[ip].imag * w.imag; // 计算t的实部
					t.imag = xin[ip].real * w.imag + xin[ip].imag * w.real; // 计算t的虚部

					/* 蝶形运算 */
					xin[ip].real = xin[i].real - t.real; // 计算xin[ip].的实部
					xin[ip].imag = xin[i].imag - t.imag; // 计算xin[ip].的虚部
					xin[i].real = xin[i].real + t.real; // 计算xin[i].的实部
					xin[i].imag = xin[i].imag + t.imag; // 计算xin[i].的虚部
				}
			}
		}
	}
}

/**
 * @brief   计算FFT结果的模值
 * @details 将复数结果转换为幅度值，存入实部
 *          公式: |X[k]| = sqrt(real^2 + imag^2) / N
 *          直流分量(k=0)除以N，其他分量除以N/2
 *
 * @param   xin FFT输出数组
 * @return  无(结果存于xin[].real中)
 *
 * @note    有效数据为前FFT_N/2个点(奈奎斯特频率以下)
 */
void Get_Result(struct compx *xin)
{
	int i = 0;
	for (i = 0; i < FFT_N / 2; ++i)
	{
		xin[i].real = sqrt(xin[i].real * xin[i].real + xin[i].imag * xin[i].imag) / (FFT_N >> (i != 0)); // 计算xin[i].的模值
	}
}

/**
 * @brief   汉宁窗函数
 * @details 对输入数据加汉宁窗，减少频谱泄漏
 *          汉宁窗公式: w(n) = 0.5 * (1 - cos(2πn/(N-1)))
 *
 *          窗函数特性:
 *          - 边缘值趋近于0，平滑过渡
 *          - 相干增益约0.5，信号幅度减半
 *          - 主瓣宽度较宽，旁瓣衰减约-31dB
 *
 * @param   xin FFT输入数组
 * @param   n   数据点数
 * @return  无(原地修改xin数组)
 */
void Hanning_Window(struct compx *xin, int n) // 汉宁窗函数
{
	int i;
	double w;
	for (i = 0; i < n; i++) // 遍历所有数据点
	{
		w = 0.5 * (1 - cos(PI2 * i / (n - 1))); // 计算汉宁窗权重
		xin[i].real = xin[i].real * w; // 加窗处理，修改xin[i].的实部
	}
}

/**
 * @brief   去除直流分量
 * @details 计算信号平均值并减去，消除直流偏移
 *          直流分量会在频谱中产生低频泄漏，影响低频分析
 *
 *          处理步骤:
 *          1. 计算所有采样点的平均值
 *          2. 每个采样点减去平均值
 *
 * @param   xin FFT输入数组
 * @param   n   数据点数
 * @return  无(原地修改xin数组)
 */
void Remove_DC(struct compx *xin, int n)
{
	int i;
	double sum = 0;
	double avg;

	/* 计算平均值 */
	for (i = 0; i < n; i++)
	{
		sum += xin[i].real;
	}
	avg = sum / n;

	/* 减去直流分量 */
	for (i = 0; i < n; i++)
	{
		xin[i].real = xin[i].real - avg;
	}
}

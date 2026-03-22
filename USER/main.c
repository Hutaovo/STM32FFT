/**
 * @file    main.c
 * @brief   STM32音频频谱分析仪主程序
 * @details 使用STM32F103C8T6通过ADC采集音频信号，进行FFT变换后在ST7789屏幕上显示频谱
 * @author  用户
 * @version 3.0
 * @date    2024
 * 
 * @note    硬件连接 (硬件SPI+DMA版本):
 *          - MAX9814麦克风模块 -> PA6 (ADC1_CH6)
 *          - MAX9814 Gain控制 -> PA11
 *          - MAX9814 AR控制   -> PB11
 *          - ST7789 LCD SCK   -> PB13 (SPI2_SCK)
 *          - ST7789 LCD MOSI  -> PB15 (SPI2_MOSI)
 *          - ST7789 LCD CS    -> PB12
 *          - ST7789 LCD DC    -> PA8
 *          - ST7789 LCD RES   -> PA9
 *          - ST7789 LCD BLK   -> PA10
 *          - SPI2 TX DMA      -> DMA1_CH5
 *          - KEY0             -> PA1 (EXTI1)
 *          - KEY1             -> PA2 (EXTI2)
 *          - KEY2             -> PA3 (EXTI3)
 *          - KEYGAIN          -> PA0
 *          - LED0             -> PB0
 *          - LED1             -> PD2
 */

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "lcd_init.h"
#include "lcd.h"
#include "adc.h"
#include "keyin.h"
#include "exti.h"
#include "fft.h"
#include "math.h"

#define N 512       /* 音频采样点数，与FFT_N保持一致 */

u8 fa = 2;          /* TIM2预分频系数，用于调节ADC采样率 */
u16 fs = 51209 / N; /* 最高采样率下的频率分辨率 = 100Hz */
u16 fra;            /* 当前ADC采样率 (Hz) */
u16 fsc;            /* 当前频率分辨率 (Hz) */

void LCD_display(void);
void Configer_GPIO_Init(void);

u16 Adc_12bit[N];   /* ADC采样数据缓冲区，存储512个采样点 */
u16 amp[N / 2];     /* 当前频谱柱状图高度数组 */
u16 preamp[N / 2];  /* 上一次频谱柱状图高度数组，用于动画效果 */
u8 Show_Flag = 0;   /* 采样率/分辨率显示开关标志 */
u8 gain_Flag = 0xff;/* MAX9814增益状态标志 */

#define MAX9814_Gain PAout(11)  /* MAX9814增益控制引脚 PA11 */
#define MAX9814_AR   PBout(11)  /* MAX9814自动增益释放时间控制引脚 PB11 */

/**
 * @brief   主函数
 * @details 初始化各外设，循环进行ADC采样、FFT变换、频谱显示
 * @param   无
 * @return  无
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    create_sin_tab(SIN_TAB);
    delay_init();
    LED_Init();
    LCD_Init();
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    EXTIX_Init();
    Configer_GPIO_Init();

    u16 s, t = 0;
    fra = 51209 / fa;
    fsc = fs / fa;
    double aMax = 0.0;
    double raMax = 0.0;
    u16 nMax = 0;
    u16 freMax = 0;
    u16 zamp[N / 2];

    LCD_ShowString(0, 0, "fre:", WHITE, BLACK, 16, 0);
    LCD_ShowString(86, 0, "Hz", WHITE, BLACK, 16, 0);
    LCD_ShowString(116, 0, "Amp:", WHITE, BLACK, 16, 0);
    LCD_ShowString(188, 0, "mV", WHITE, BLACK, 16, 0);
    LCD_ShowString(0, 16, "fra:", WHITE, BLACK, 16, 0);
    LCD_ShowString(86, 16, "Hz", WHITE, BLACK, 16, 0);
    LCD_ShowString(116, 16, "fsc:", WHITE, BLACK, 16, 0);
    LCD_ShowIntNum(32, 16, fra, 5, RED, BLACK, 16);
    LCD_ShowIntNum(148, 16, fsc, 3, RED, BLACK, 16);

    TIM2_PWM_Init(1406, fa - 1);
    ADC_DMA1_Config(DMA1_Channel1, (uint32_t)&ADC1->DR, (uint32_t)&Adc_12bit, N);
    Adc_Init();

    while (1)
    {
        if (KEYGAIN == 0)
        {
            gain_Flag = ~gain_Flag;
            LED1 = ~LED1;
            if (gain_Flag == 0)
                MAX9814_Gain = 0;
            else
                MAX9814_Gain = 1;
            while (KEYGAIN == 0)
                ;
        }
        for (t = 0; t < N; t++)
        {
            Compx[t].real = Adc_12bit[t];
            Compx[t].imag = 0;
        }
        Remove_DC(Compx, N);
        Hanning_Window(Compx, N);
        FFT(Compx);
        Get_Result(Compx);

        aMax = 0.0;
        nMax = 0;
        for (s = 0; s < N / 2 - 1; s++)
        {
            if (Compx[s + 1].real > aMax)
            {
                aMax = Compx[s + 1].real;
                nMax = s + 1;
            }
            zamp[s] = (u16)(Compx[s + 1].real * 0.4);
            if (zamp[s] > 207)
                zamp[s] = 207;
            amp[s] = 239 - zamp[s];
        }
        LCD_display();
        freMax = nMax * fsc;
        raMax = (u16)(aMax * 0.8057);

        LCD_ShowIntNum(32, 0, freMax, 5, RED, BLACK, 16);
        LCD_ShowIntNum(148, 0, raMax, 4, RED, BLACK, 16);
        if (Show_Flag)
        {
            LCD_ShowIntNum(32, 16, fra, 5, RED, BLACK, 16);
            LCD_ShowIntNum(148, 16, fsc, 3, RED, BLACK, 16);
        }
    }
}

/**
 * @brief   EXTI1外部中断服务函数
 * @details KEY0按下时切换采样率/分辨率显示状态
 * @param   无
 * @return  无
 */
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        delay_ms(5);
        if (KEY0 == 0)
        {
            Show_Flag = !Show_Flag;
            LED0 = !LED0;
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

/**
 * @brief   EXTI2外部中断服务函数
 * @details KEY1按下时减小ADC采样率，增大频率分辨率
 * @param   无
 * @return  无
 */
void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        delay_ms(5);
        if (KEY1 == 0)
        {
            fa++;
            TIM_PrescalerConfig(TIM2, fa - 1, TIM_PSCReloadMode_Immediate);
            fra = 51209 / fa;
            fsc = fs / fa;
        }
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

/**
 * @brief   EXTI3外部中断服务函数
 * @details KEY2按下时增大ADC采样率，减小频率分辨率
 * @param   无
 * @return  无
 */
void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        delay_ms(5);
        if (KEY2 == 0)
        {
            fa--;
            if (fa < 1)
                fa = 1;
            TIM_PrescalerConfig(TIM2, fa - 1, TIM_PSCReloadMode_Immediate);
            fra = 51209 / fa;
            fsc = fs / fa;
        }
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

/**
 * @brief   频谱柱状图绘制函数
 * @details 在LCD上绘制前80个频率点的柱状图，带有动画效果
 *          - 绿色: 当前幅值
 *          - 蓝色: 上一次幅值(下降动画)
 * @param   无
 * @return  无
 */
void LCD_display(void)
{
    u8 f;
    for (f = 0; f < 80; f++)
    {
        if (amp[f] > preamp[f])
        {
            LCD_Fillwidth(3 * f, preamp[f] - 2, 2, amp[f], BLACK);
            LCD_Fillwidth(3 * f, amp[f] - 2, 2, amp[f], GREEN);
        }
        else
        {
            LCD_Fillwidth(3 * f, amp[f] - 2, 2, amp[f], GREEN);
            LCD_Fillwidth(3 * f, amp[f], 2, preamp[f], BLUE);
        }
        preamp[f] = amp[f];
    }
}

/**
 * @brief   MAX9814控制引脚初始化函数
 * @details 配置PA11和PB11为推挽输出，用于控制MAX9814麦克风模块
 *          - PA11: Gain引脚，控制增益(0=低增益，1=高增益)
 *          - PB11: AR引脚，控制自动增益释放时间
 * @param   无
 * @return  无
 */
void Configer_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

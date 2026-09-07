/*
 * ultrasonic.h —— HC-SR04 超声波驱动（TIM4 PWM输入捕获），【下位机侧配套代码】
 *
 * ★ 引脚依据：智能车驱动板 V1.0（2026-09-06 更新）原理图 + BOM
 *   板上“XH2.54 超声波”4P 座(XH-4A)：
 *       pin1 ECHO -> PB6 = TIM4_CH1
 *       pin2 TRIG -> PB7（普通推挽输出即可）
 *       pin3 GND，pin4 +5V
 *   PB6/PB7 正好是 TIM4_CH1/CH2，用 PWM 输入捕获测回波脉宽。
 *
 *   注意：旧版 servo.c 把 TIM4(PB6~PB9) 当舵机 PWM，是【旧板】布线；
 *   现板舵机信号在 DJ0~DJ5 = PC6~PC9(TIM8_CH1~4)、PA0/PA1(TIM2_CH1/2)，
 *   舵机改到 TIM8/TIM2 后，TIM4 即归超声波专用，互不冲突。
 *
 * 说明：旧 line.c/line.h 与 GetDistance() 已删除，模式4（视觉搬运）统一用本驱动
 *       非阻塞测距，由原位修改后的 mode4_handle() 每 50ms 上报 @Dxxxx# 给 OpenMV
 *       （具体改法见 mode4_inplace_edit.c）。
 */
#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "main.h"

/* Trig = PB7（与原理图一致，一般不用改） */
#define ULTRA_TRIG_PORT     GPIOB
#define ULTRA_TRIG_PIN      GPIO_PIN_7

void    Ultrasonic_Init(void);                 /* main 初始化时调用一次 */
void    Ultrasonic_StartOnce(void);            /* 触发一次测距（非阻塞） */
int32_t Ultrasonic_GetDistance_cm(void);       /* 最近一次距离(cm)，-1=无效 */
void    Ultrasonic_Tick(uint32_t now_ms,
                        uint32_t period_ms);   /* 主循环周期触发，period=50ms */
void    Ultrasonic_IC_Callback(TIM_HandleTypeDef *htim); /* 捕获中断里转调 */

#endif /* __ULTRASONIC_H */

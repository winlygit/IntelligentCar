
#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "main.h"

#define ULTRA_TRIG_PORT     GPIOB
#define ULTRA_TRIG_PIN      GPIO_PIN_7

void    Ultrasonic_Init(void);                 /* main 初始化时调用一次 */
void    Ultrasonic_StartOnce(void);            /* 触发一次测距（非阻塞） */
int32_t Ultrasonic_GetDistance_cm(void);       /* 最近一次距离(cm)，-1=无效 */
void    Ultrasonic_Tick(uint32_t now_ms,
                        uint32_t period_ms);   /* 主循环周期触发，period=50ms */
void    Ultrasonic_IC_Callback(TIM_HandleTypeDef *htim); /* 捕获中断里转调 */

#endif /* __ULTRASONIC_H */

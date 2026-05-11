
/*
******************************************************************************
    本文件主要用于存放舵机驱动函数
    基于global里的全局变量，将计算后的数据转化为IO指令
    
    
******************************************************************************
*/
#include "servo.h"
#include "main.h"
#include "global.h"
#include "stdio.h"

void Servo_SetAngle(uint8_t channel, uint16_t angle)
{
    if (angle > 270)
        angle = 270;
    uint32_t pulse = 500 + ((uint32_t)angle * 2000) / 270;
    switch (channel)
    {
    case SERVO_CH_PB3:
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
        break;
    case SERVO_CH_PB4:
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
        break;
    case SERVO_CH_PB6:
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pulse);
        break;
    case SERVO_CH_PB7:
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pulse);
        break;
    case SERVO_CH_PB8:
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, pulse);
        break;
    case SERVO_CH_PB9:
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, pulse);
        break;
    default:
        break;
    }
}





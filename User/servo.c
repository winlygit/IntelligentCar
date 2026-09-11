
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
    case SERVO_CH_PC6:
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, pulse);
        break;
    case SERVO_CH_PC7:
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, pulse);
        break;
    case SERVO_CH_PC8:
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, pulse);
        break;
    case SERVO_CH_PC9:
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, pulse);
        break;
    case SERVO_CH_PA0:
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse);
        break;
    case SERVO_CH_PA1:
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
        break;
    default:
        break;
    }
}

void Servo_Sendcmd(servoANGLE *servoangle){
    Servo_SetAngle(SERVO_CH_PC6,servoangle->D1);
    Servo_SetAngle(SERVO_CH_PC7,servoangle->D2);
    Servo_SetAngle(SERVO_CH_PC8,servoangle->D3);
    Servo_SetAngle(SERVO_CH_PC9,servoangle->D4);
    Servo_SetAngle(SERVO_CH_PA0,servoangle->D5);
    Servo_SetAngle(SERVO_CH_PA1,servoangle->D6);
}

void servo_init(void){
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    Servo_SetAngle(SERVO_CH_PC6,135);
	Servo_SetAngle(SERVO_CH_PC7,135);
	Servo_SetAngle(SERVO_CH_PC8,135);
	Servo_SetAngle(SERVO_CH_PC9,135);
	Servo_SetAngle(SERVO_CH_PA0,135);
	Servo_SetAngle(SERVO_CH_PA1,202);
}





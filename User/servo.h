#ifndef __SERVO_H
#define __SERVO_H


#include "main.h"
#include "global.h"

#include "stm32f1xx_hal.h"
#define SERVO_CH_PB3  1
#define SERVO_CH_PB4  2
#define SERVO_CH_PB6  3
#define SERVO_CH_PB7  4
#define SERVO_CH_PB8  5
#define SERVO_CH_PB9  6

void Servo_SetAngle(uint8_t channel, uint16_t angle);
void servo_init(void);//初始化
void Servo_Sendcmd(void);







#endif

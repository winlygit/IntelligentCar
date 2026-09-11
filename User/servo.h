#ifndef __SERVO_H
#define __SERVO_H


#include "main.h"
#include "global.h"

#include "stm32f1xx_hal.h"
#define SERVO_CH_PC6  1
#define SERVO_CH_PC7  2
#define SERVO_CH_PC8  3
#define SERVO_CH_PC9  4
#define SERVO_CH_PA0  5
#define SERVO_CH_PA1  6

void Servo_SetAngle(uint8_t channel, uint16_t angle);
void servo_init(void);//初始化
void Servo_Sendcmd(servoANGLE *servoangle);







#endif

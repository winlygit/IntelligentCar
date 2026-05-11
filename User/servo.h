#ifndef __SERVO_H
#define __SERVO_H


#include "main.h"
#include "global.h"

#include "stm32f1xx_hal.h"
#define SERVO_CH_PB3  0
#define SERVO_CH_PB4  1
#define SERVO_CH_PB6  2
#define SERVO_CH_PB7  3
#define SERVO_CH_PB8  4
#define SERVO_CH_PB9  5

void Servo_SetAngle(uint8_t channel, uint16_t angle); // angle: 0~270







#endif

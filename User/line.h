#ifndef __LINE_H
#define __LINE_H

#include "main.h"
#include "global.h"

#define L_PIN    GPIO_PIN_0
#define R_PIN    GPIO_PIN_1
#define PORT     GPIOA

#define IN_LINE        0
#define NO_LINE     1

void LINE_Track(motorSPEED *motorspeed, int16_t speed);  // 循迹控制函数

#endif
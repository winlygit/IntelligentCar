#ifndef __IK_H
#define __IK_H


#include "main.h"
#include "global.h"

void motor_ik(void);//将速度量转化为单个轮子的速度,范围（-1000 ， 1000）
void servo_ik(void);             //将机械臂末端位置转化为舵机控制量，范围-135-135






#endif

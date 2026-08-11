#ifndef __IK_H
#define __IK_H


#include "main.h"
#include "global.h"

void motor_ik(motorSPEED *motorspeed,MDATA *speedData_primary); //将速度量转化为单个轮子的速度,范围（-1000 ， 1000）
void servo_ik(servoANGLE *servoangle,SDATA *servoData_primary); //将机械臂末端位置转化为舵机控制量，范围-135-135

void motor_stop(motorSPEED *motorspeed);
void servo_stop(servoANGLE *servoangle);



#endif

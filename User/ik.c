
/*
******************************************************************************
    本文件主要用于运动学解算
    基于global里的数据，将数据转化为直观的控制量
    
    
******************************************************************************
*/


#include "main.h"
#include "global.h"
#include "uart.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>





/****************有关麦肯纳姆轮的逆解算法*****************/


 void motor_ik(void){             //将速度量转化为单个轮子的速度,范围（-1000 ， 1000）
    
    motorspeed.LFsd =  (speed.Vx+speed.Vy-speed.Wz)*1000/Add;
    motorspeed.RFsd = (-speed.Vx+speed.Vy+speed.Wz)*1000/Add;
    motorspeed.LBsd = (-speed.Vx+speed.Vy-speed.Wz)*1000/Add;
    motorspeed.RBsd =  (speed.Vx+speed.Vy+speed.Wz)*1000/Add;

    
    int V[4] = {motorspeed.LFsd , motorspeed.RFsd , motorspeed.LBsd , motorspeed.RBsd};
    int MAX_MEM = abs(V[0]);         //找到轮速最大值
    for(int i=1;i<4;i++){
        if(MAX_MEM < abs(V[i])){
        MAX_MEM = abs(V[i]);
        }
    }
    
    
    float b = sqrt(speed.Vx*speed.Vx + speed.Vy*speed.Vy + speed.Wz*speed.Wz)/90;        //定义速度程度参数
    if(b > 1){
        b = 1;
    }
    
    
    float k = b*1000/MAX_MEM;             //缩放比例
    
    motorspeed.LFsd *= k;
    motorspeed.RFsd *= k;
    motorspeed.LBsd *= k;
    motorspeed.RBsd *= k;

    
    
}




/****************有关机械臂的逆解算法*****************/









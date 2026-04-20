
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


motorSPEED motor_ik(SPEED spd){             //将速度量转化为单个轮子的速度,范围（-1000 ， 1000）
    motorSPEED mospd;
    mospd.LFsd =  (spd.Vx+spd.Vy-spd.Wz)*1000/Add;
    mospd.RFsd = (-spd.Vx+spd.Vy+spd.Wz)*1000/Add;
    mospd.LBsd = (-spd.Vx+spd.Vy-spd.Wz)*1000/Add;
    mospd.RBsd =  (spd.Vx+spd.Vy+spd.Wz)*1000/Add;

    
    int V[4] = {mospd.LFsd , mospd.RFsd , mospd.LBsd , mospd.RBsd};
    int MAX_MEM = abs(V[0]);         //找到轮速最大值
    for(int i=1;i<4;i++){
        if(MAX_MEM < abs(V[i])){
        MAX_MEM = abs(V[i]);
        }
    }
    
    
    float b = sqrt(spd.Vx*spd.Vx + spd.Vy*spd.Vy + spd.Wz*spd.Wz)/90;        //定义速度程度参数
    if(b > 1){
        b = 1;
    }
    
    
    float k = b*1000/MAX;             //缩放比例
    
    mospd.LFsd *= k;
    mospd.RFsd *= k;
    mospd.LBsd *= k;
    mospd.RBsd *= k;

    
    return mospd;
}




/****************有关机械臂的逆解算法*****************/









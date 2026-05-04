
/*
******************************************************************************
    本文件主要用于存放电机驱动函数
    基于global里的全局变量，将计算后的数据转化为IO指令
    
    
******************************************************************************
*/


#include "main.h"
#include "global.h"
#include <stdio.h>
#include "uart.h"



void SendCmd(void){                 //将速度通过总线发送到电机
    int LF,RF,LB,RB;   
    LF = 1500 + motorspeed.LFsd;
    RF = 1500 + motorspeed.RFsd;
    LB = 1500 + motorspeed.LBsd;
    RB = 1500 + motorspeed.RBsd;
    
    
    char cmd[61];
    sprintf(cmd,"#001P%04dT0000!#002P%04dT0000!#003P%04dT0000!#004P%04dT0000!",LF,RF,LB,RB);
    
    
    U3_printf((uint8_t*)cmd);
}



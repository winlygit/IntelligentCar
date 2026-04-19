
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



void SendCmd(motorSPEED MSPD){                 //将速度通过总线发送到电机
    int LF,RF,LB,RB;   
    LF = 1500 + MSPD.LFsd;
    RF = 1500 + MSPD.RFsd;
    LB = 1500 + MSPD.LBsd;
    RB = 1500 + MSPD.RBsd;
    
    
    char cmd[60];
    sprintf(cmd,"#001P%04dT0000!#002P%04dT0000!#003P%04dT0000!#004P%04dT0000!\r\n",LF,RF,LB,RB);
    
    
    U3_printf((uint8_t*)cmd);
 
    
}



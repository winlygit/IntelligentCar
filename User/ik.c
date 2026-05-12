
/*
******************************************************************************
    本文件主要用于运动学解算
    基于global里的数据，将数据转化为直观的控制量
    
    
******************************************************************************
*/


#include "main.h"
#include "global.h"
#include "uart.h"
#include "readdata.h"
#include "ik.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>





/****************有关麦肯纳姆轮的逆解算法*****************/


 void motor_ik(void){             //将速度量转化为单个轮子的速度,范围（-1000 ， 1000）
    
    motorspeed.LFsd =  (speedData_primary.Vx+speedData_primary.Vy-speedData_primary.Wz)*1000/Add;
    motorspeed.RFsd = (-speedData_primary.Vx+speedData_primary.Vy+speedData_primary.Wz)*1000/Add;
    motorspeed.LBsd = (-speedData_primary.Vx+speedData_primary.Vy-speedData_primary.Wz)*1000/Add;
    motorspeed.RBsd =  (speedData_primary.Vx+speedData_primary.Vy+speedData_primary.Wz)*1000/Add;

    
    int V[4] = {motorspeed.LFsd , motorspeed.RFsd , motorspeed.LBsd , motorspeed.RBsd};
    int MAX_MEM = abs(V[0]);         //找到轮速最大值
    for(int i=1;i<4;i++){
        if(MAX_MEM < abs(V[i])){
        MAX_MEM = abs(V[i]);
        }
    }
    
    
    float b = sqrt(speedData_primary.Vx*speedData_primary.Vx + speedData_primary.Vy*speedData_primary.Vy + speedData_primary.Wz*speedData_primary.Wz)/90;        //定义速度程度参数
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

void servo_ik(void){

    //对D1的处理
    HAL_Delay(5);
    if(servoData_primary.D1 >= 0 && servoData_primary.D1 <= 30){     //每10ms变化0.2度
        botom_angle +=0.2;
       
    }else if(servoData_primary.D1 > 60 && servoData_primary.D1 <= 90){
        botom_angle -=0.2;
    }

    if(botom_angle > 270){          //死区限制
        botom_angle = 270;
    }else if(botom_angle < 0){
        botom_angle = 0;
    }
    servoangle.D1 = botom_angle;

    //对D5的处理
    HAL_Delay(5);
    if(servoData_primary.D5 >= 0 && servoData_primary.D5 <= 30){     //每10ms变化0.2度
        circle_angle +=0.2;
       
    }else if(servoData_primary.D5 > 60 && servoData_primary.D5 <= 90){
        circle_angle -=0.2;
    }
    
    if(circle_angle > 225){          //死区限制
        circle_angle = 225;
    }else if(circle_angle < 45){
        circle_angle = 45;
    }
    servoangle.D5 = circle_angle ;

    //对D6的处理
    if(servoData_primary.D6 == 1){       //占空比0.25对应夹爪打开，0.75对应夹爪关闭，68-202为实际控制量范围
        servoangle.D6 = 140;
    }else if(servoData_primary.D6 == 0){
        servoangle.D6 = 68;
    }

    //对D2-D4的处理
    servoangle.D2 = servoData_primary.D2 + 135;
    servoangle.D3 = servoData_primary.D3 + 135;
    servoangle.D4 = servoData_primary.D4 + 135;
}







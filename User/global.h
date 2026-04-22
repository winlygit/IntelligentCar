#ifndef __GLOBAL_H
#define __GLOBAL_H


#include "main.h"

/****************存放main中的宏定义和全局变量*****************/















/****************存放uart中的宏定义和全局变量*****************/
#define MAX 256        //最大接收量
#define BlueState HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)











/****************存放readdata中的宏定义和全局变量*****************/
#define WW 23     //控制wz的数据的位置
#define Vz 90    //控制z轴旋转速度的量
#define limit 20      //角度死区

typedef struct {                     //原始数据格式
    int Vx,Vy,Wz;
}SPEED;

typedef struct  {                //计算后电机数据格式
    int LFsd;         //左前，对应电机id 001
    int RFsd;         //右前，对应电机id 002
    int LBsd;         //左后，对应电机id 003
    int RBsd;         //右后，对应电机id 004
}motorSPEED;

extern SPEED speed;          //解析数据帧得到的原始陀螺仪数据
extern motorSPEED motorspeed;          //计算后得到的电机相对转速，范围（-1000，1000）
extern uint8_t RxData[MAX];          //上位机原始数据帧存放处












/****************存放ik中的宏定义和全局变量*****************/
#define Add 270       //数据和的最大值，用于确定程度














/****************存放servo中的宏定义和全局变量*****************/


















/****************存放motor中的宏定义和全局变量*****************/














/****************存放buzzer和led中的宏定义和全局变量*****************/












































#endif

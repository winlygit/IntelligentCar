#ifndef __GLOBAL_H
#define __GLOBAL_H


#include "main.h"

/****************存放main中的宏定义和全局变量*****************/















/****************存放uart中的宏定义和全局变量*****************/
#define MAX 256        //最大接收量
#define BlueState HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)

extern uint8_t RxData[MAX];          //上位机原始数据帧存放处

extern uint8_t value;     //上位机发来的字节
extern uint16_t volatile uart_index;    //读到位
extern uint8_t volatile ifrxstart;
extern uint8_t volatile rxcplt_flag;      //数据读完标志位











/****************存放readdata中的宏定义和全局变量*****************/
#define WW 25     //控制wz的数据的位置
#define Vz 60    //控制z轴旋转速度的量
#define limit 20      //角度死区


typedef struct {                     //原始数据格式
    int Vx,Vy,Wz;
}MDATA;



typedef struct {                 //原始数据格式
    int D1;           //底盘舵机  0-90
    int D2;           //舵机2     -135-135
    int D3;           //舵机3     -135-135
    int D4;           //舵机4     -135-135
    int D5;           //手腕舵机    0-90
    uint8_t D6;           //夹爪舵机    为01值
}SDATA;


extern uint8_t actionnum;        //动作组中的动作编号
extern MDATA speedData_primary;          //解析数据帧得到的原始陀螺仪数据
extern SDATA servoData_primary;    //上位机发来的六个舵机原始控制量
extern uint8_t RxData[MAX];          //上位机原始数据帧存放处












/****************存放ik中的宏定义和全局变量*****************/

#define Add 270       //数据和的最大值，用于确定程度

typedef struct  {                //计算后电机数据格式
    int LFsd;         //左前，对应电机id 001
    int RFsd;         //右前，对应电机id 002
    int LBsd;         //左后，对应电机id 003
    int RBsd;         //右后，对应电机id 004
}motorSPEED;


typedef struct {         //计算后舵机数据格式
    int D1;           //舵机1     -135-135
    int D2;           //舵机2     -135-135
    int D3;           //舵机3     -135-135
    int D4;           //舵机4     -135-135
    int D5;           //舵机5     -135-135
    int D6;           //舵机6     -135-135
}servoANGLE;




extern motorSPEED motorspeed;          //计算后得到的电机相对转速，范围（-1000，1000）
extern servoANGLE servoangle;        //计算后得到的舵机控制量，范围-135-135




/****************存放servo中的宏定义和全局变量*****************/


















/****************存放motor中的宏定义和全局变量*****************/














/****************存放buzzer和led中的宏定义和全局变量*****************/












































#endif

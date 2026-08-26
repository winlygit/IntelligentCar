#ifndef __GLOBAL_H
#define __GLOBAL_H


#include "main.h"

/****************存放main中的宏定义和全局变量*****************/















/****************存放uart中的宏定义和全局变量*****************/
#define MAX 256        //最大接收量
#define BlueState HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)     //蓝牙状态检测引脚

extern uint8_t RxData[MAX];          //上位机原始数据帧存放处

extern uint8_t value;     //上位机发来的字节
extern uint16_t volatile uart_index;    //读到位
extern uint8_t volatile ifrxstart;       //是否开始读数据的标志位
extern uint8_t volatile rxcplt_flag;      //数据读完标志位











/****************存放readdata中的宏定义和全局变量*****************/
#define Vz 60    //控制z轴旋转速度的量
#define limit 20      //角度死区


typedef struct {                     //原始陀螺仪数据格式
    int Vx,Vy,Wz;
}MDATA;



typedef struct {                 //原始舵机数据格式
    int D1;           //底盘舵机  0-90
    int D2;           //舵机2     -135-135
    int D3;           //舵机3     -135-135
    int D4;           //舵机4     -135-135
    int D5;           //手腕舵机    0-90
    uint8_t D6;           //夹爪舵机    为01值
}SDATA;

typedef enum{   //模式枚举变量
    STOP = 0,
    mode1,
    mode2,
    mode3,
    mode4,
}MODE;

//模式1和模式2数据帧包含的所有数据
typedef struct {

    uint8_t IFSTOP;        //是否退出当前模式,0=不退出，1=退出
    uint8_t STATUS;        //当前模式的状态,0=停止，1=运行
    MDATA speedData_primary;          //当前模式下的陀螺仪数据
    SDATA servoData_primary;    //当前模式下的六个舵机控制量
}mode1_data;

//模式1和模式2数据帧包含的所有数据
typedef struct {

    uint8_t IFSTOP;        //是否退出当前模式
    uint8_t STATUS;        //当前模式的状态
    MDATA speedData_primary;          //当前模式下的陀螺仪数据
    SDATA servoData_primary;    //当前模式下的六个舵机控制量
    uint8_t NAME[16];          //模式二动作组名称
}mode2_data;

//模式3数据帧包含的所有数据
typedef struct {
    uint8_t IFSTOP;        //是否退出当前模式
    uint8_t IFREFRESH;     //是否请求刷新动作组目录
    uint8_t IFEXECUTE;     //是否请求执行动作组
    uint8_t ACTIONID;       //请求执行的动作组编号
    uint8_t IFDELETE;       //是否请求删除动作组
    uint8_t DELETEID;       //请求删除的动作组编号
    uint8_t STATUS;        // 当前模式的状态（0：空闲，1：有任务正执行）

}mode3_data;

//模式4数据帧包含的所有数据
typedef struct {
    uint8_t IFSTOP;        //是否退出当前模式
    uint8_t IFREFRESH;     //是否请求刷新动作组目录
    uint8_t STATUS;        //当前模式的状态,是否循迹
    uint8_t ACTIONID;       //请求执行的动作组编号
    uint8_t IFREACH;         //是否达到执行动作组的距离阈值
    uint16_t DISTANCE;        //距离阈值

}mode4_data;



extern uint8_t RxData[MAX];          //上位机原始数据帧存放处












/****************存放ik中的宏定义和全局变量*****************/

#define Add 270       //数据和的最大值，用于确定程度
#define ANGLE 2       //地盘和夹爪每10ms变化的角度量

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
extern float botom_angle;         //底盘舵机控制量，范围0-270
extern float circle_angle;         //手腕舵机控制量，范围45-225








/****************存放line中的宏定义和全局变量*****************/
extern int tick_signal;  //用于巡线的定时器信号量









































#endif

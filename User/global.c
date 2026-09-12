
/*
******************************************************************************
本文件用于存放全局变量


******************************************************************************
*/


#include "main.h"
#include "global.h"



/****************存放main中的全局变量*****************/















/****************存放uart中的全局变量*****************/
uint8_t RxData_Phone[MAX];          //上位机原始数据帧存放处
uint8_t RxData_Openmv[MAX];          //openmv原始数据帧存放处

uint8_t value_Phone;     //上位机发来的字节
uint8_t value_Openmv;    //openmv发来的字节

uint16_t volatile uart_index_Phone;    //读到位
uint16_t volatile uart_index_Openmv;    //读到位

uint8_t volatile ifrxstart_Phone;       //是否开始读数据的标志位
uint8_t volatile ifrxstart_Openmv;       //是否开始读数据的标志

uint8_t volatile rxcplt_flag_Phone = 0;      //数据读完标志位
uint8_t volatile rxcplt_flag_Openmv = 0;      //数据读完标志位









/****************存放readdata中的全局变量*****************/
















/****************存放ik中的全局变量*****************/
float botom_angle = 135;         //底盘舵机控制量，范围0-270

float circle_angle = 135;         //手腕舵机控制量，范围45-225












/****************存放servo中的全局变量*****************/


















/****************存放motor中的全局变量*****************/













/************存放flash和actiongroup中的全局变量*************/




/************存放line中的全局变量*************/
int tick_signal; 




















#ifndef __READDATA_H
#define __READDATA_H


#include "main.h"
#include "global.h"







SPEED motor_readdata (uint8_t Data[]);              //电机控制数据帧解析

int readindexdata3(uint8_t data[],int index1,int index2,int index3);

int readindexdata4(uint8_t data[],int index1,int index2,int index3,int index4);










#endif

#ifndef __READDATA_H
#define __READDATA_H


#include "main.h"
#include "global.h"









MODE readmode (uint8_t *Data);              //模式解析函数
void readdata1(mode1_data *data,uint8_t *Data);              //模式1数据解析函数
void readdata2(mode2_data *data,uint8_t *Data);              //模式2数据解析函数
void readdata3(mode3_data *data,uint8_t *Data);              //模式3数据解析函数
void readdata4(mode4_data *data,uint8_t *Data);              //模式4数据解析函数
int IFREACH_check(int16_t actual_distance, int16_t threshold);        //模式4检查是否达到距离阈值











#endif

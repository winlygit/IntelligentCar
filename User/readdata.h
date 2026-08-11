#ifndef __READDATA_H
#define __READDATA_H


#include "main.h"
#include "global.h"









MODE readmode (uint8_t *Data);              //模式解析函数
void readdata12(mode12_data *data,uint8_t *Data);              //模式1和模式2数据解析函数
void readdata3(mode3_data *data,uint8_t *Data);              //模式3数据解析函数
void readdata4(mode4_data *data,uint8_t *Data);              //模式4数据解析函数











#endif

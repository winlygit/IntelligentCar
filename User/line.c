/*
******************************************************************************
    本文件用于两路红外循迹传感器驱动
    左传感器：PA0，右传感器：PA1
    检测到黑线时输出低电平，白线输出高电平
******************************************************************************
*/
#include "main.h"
#include "global.h"
#include "line.h"


// 读左传感器：返回 LINE 或 NO_LINE
static uint8_t ReadLeft(void)
{
    return HAL_GPIO_ReadPin(PORT, L_PIN);
}

// 读右传感器
static uint8_t ReadRight(void)
{
    return HAL_GPIO_ReadPin(PORT, R_PIN);
}


void LINE_Track(motorSPEED *motorspeed, int16_t speed)
{
    uint8_t l = ReadLeft();
    uint8_t r = ReadRight();

    int16_t ls = speed, rs = speed;

    if (l == IN_LINE && r == NO_LINE)      // 车偏右，左转：减速左轮
        ls = speed / 2;
    else if (r == IN_LINE && l == NO_LINE) // 车偏左，右转：减速右轮
        rs = speed / 2;
    else if (l == IN_LINE && r == IN_LINE);    // 都在线上，直行
    else                                // 都脱线，停车
    {
        ls = 0;
        rs = 0;
    }

    motorspeed->LFsd = ls;
    motorspeed->RFsd = rs;
    motorspeed->LBsd = ls;
    motorspeed->RBsd = rs;
}
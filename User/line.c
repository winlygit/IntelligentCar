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


/*以下是传感器部分*/

static volatile uint32_t echo_start_tick = 0;
static volatile uint32_t echo_end_tick   = 0;
static volatile uint8_t  echo_rising     = 0;
static volatile uint8_t  echo_done       = 0;

static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static void Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while((DWT->CYCCNT - start) < ticks);
}

/* 触发测距 */
static void ultrasonic_Trigger(void)
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
    Delay_us(2);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    Delay_us(10);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

void ultrasonic_Init(void)
{
    DWT_Init();  //初始化DWT
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == ECHO_PIN)
    {
        if (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET)
        {
            //读上升沿
            echo_start_tick = DWT->CYCCNT;
            echo_rising = 1;
        }
        else
        {
            //读下降沿
            if (echo_rising)
            {
                echo_end_tick = DWT->CYCCNT;
                echo_done = 1;
                echo_rising = 0;
            }
        }
    }
}

int32_t GetDistance(void)
{
    uint32_t pulse_ticks;
    uint32_t timeout_tick;

    echo_done   = 0;
    echo_rising = 0;

    ultrasonic_Trigger();

    timeout_tick = DWT->CYCCNT + SystemCoreClock / 20; // 约50ms超时
    while (!echo_done)
    {
        if ((DWT->CYCCNT - timeout_tick) < 0x80000000U) // 检测超时
        {
            return -1; //超时，返回-1表示测距失败
        }
    }
    pulse_ticks = echo_end_tick - echo_start_tick;
    uint32_t us = pulse_ticks / (SystemCoreClock / 1000000);
    return (us / 58);
}
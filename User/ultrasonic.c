/*
 * ultrasonic.c —— HC-SR04 超声波驱动（TIM4 PWM输入捕获），【下位机侧配套代码】
 *
 * 硬件（驱动板 V1.0 原理图）：
 *   Echo -> PB6 = TIM4_CH1（PWM输入：CH1直接模式 + CH2间接模式，从模式复位）
 *   Trig -> PB7（GPIO 推挽输出，发 10us 脉冲）
 *
 * CubeMX 配置步骤：
 *   1) Timers -> TIM4：
 *      Combined Channels -> PWM Input on CH1(PB6)
 *      Clock Source: Internal；Prescaler = 72-1（72MHz/72=1MHz，1 tick=1us）
 *      Counter Period = 65535；
 *      CH1: PWM Input Mode direct  (TI1FP1，上升沿复位计数器并锁周期)
 *      CH2: PWM Input Mode indirect(TI1FP1，下降沿锁脉宽)
 *      NVIC：TIM4 global interrupt 打勾使能
 *   2) PB7 配 GPIO_Output(Push-Pull, no pull, low)，用户标签可写 TRIG
 *   3) main 初始化里调用 Ultrasonic_Init()
 *   4) 在工程【已有】的 HAL_TIM_IC_CaptureCallback() 里转调
 *      Ultrasonic_IC_Callback(htim)；若工程没有该回调，把本文件末尾注释解开
 *      （全工程只能存在一个 HAL_TIM_IC_CaptureCallback）。
 */
#include "ultrasonic.h"
#include "stm32f1xx_hal.h"

#define ECHO_TIMEOUT_MS     30      /* 回波超时：约 5m，超过判无效 */
#define US_PER_CM           58.8f   /* 340m/s：1cm 往返对应 58.8us */

static volatile uint32_t s_echo_us = 0;
static volatile uint8_t  s_new = 0;
static uint32_t s_trig_tick = 0;
static int32_t  s_last_cm = -1;

/* DWT 微秒级延时，用来发 10us Trig 脉冲（Cortex-M3 可用） */
static void dwt_delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks) { }
}

static void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void Ultrasonic_Init(void)
{
    dwt_init();
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
    HAL_GPIO_WritePin(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, GPIO_PIN_RESET);
}

void Ultrasonic_StartOnce(void)
{
    HAL_GPIO_WritePin(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, GPIO_PIN_SET);
    dwt_delay_us(12);
    HAL_GPIO_WritePin(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, GPIO_PIN_RESET);
    s_trig_tick = HAL_GetTick();
    s_new = 0;
}

int32_t Ultrasonic_GetDistance_cm(void)
{
    return s_last_cm;
}

void Ultrasonic_Tick(uint32_t now_ms, uint32_t period_ms)
{
    /* 非阻塞：到点触发一次；回波回来后在中断里更新 s_last_cm */
    static uint32_t last = 0;
    if (now_ms - last >= period_ms) {
        last = now_ms;
        Ultrasonic_StartOnce();
    }
    if (s_new) {
        s_new = 0;
        uint32_t us = s_echo_us;
        if (us == 0 || us > 30000) {          /* 异常脉宽剔除 */
            s_last_cm = -1;
        } else {
            s_last_cm = (int32_t)(us / US_PER_CM + 0.5f);
            if (s_last_cm > 400) s_last_cm = -1;
        }
    } else if (s_trig_tick != 0 &&
               HAL_GetTick() - s_trig_tick > ECHO_TIMEOUT_MS) {
        s_last_cm = -1;                        /* 超时无回波 */
    }
}

void Ultrasonic_IC_Callback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM4) return;
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
        /* 复位模式下 CCR2 即高电平脉宽(tick=us)；CCR1 为周期，这里不用 */
        s_echo_us = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
        s_new = 1;
    }
}

/*
 * 若工程里没有 HAL_TIM_IC_CaptureCallback，解开下面这段（只能存在一份）：
 *
 * void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
 * {
 *     Ultrasonic_IC_Callback(htim);
 * }
 */

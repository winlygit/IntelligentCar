#include "ultrasonic.h"
#include "stm32f1xx_hal.h"

#define ECHO_TIMEOUT_MS     30      /* 回波超时：约 5m，超过判无效 */
#define US_PER_CM           58.8f   /* 340m/s：1cm 往返对应 58.8us */

static volatile uint32_t s_echo_us = 0;
static volatile uint8_t  s_new = 0;
static uint32_t s_trig_tick = 0;
static int32_t  s_last_cm = -1;

/* DWT 微秒级延时，用来发 10us Trig 脉冲 */
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
 * 若工程里没有 HAL_TIM_IC_CaptureCallback，解开下面这段：
 *
 * void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
 * {
 *     Ultrasonic_IC_Callback(htim);
 * }
 */

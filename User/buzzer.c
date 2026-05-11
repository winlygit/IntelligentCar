#include "main.h"
#include "buzzer.h"

#define Buzzer_Autoreload  1000

void Buzzer_On(){
    __HAL_TIM_SET_AUTORELOAD(&htim3, Buzzer_Autoreload);
__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, Buzzer_Autoreload / 2);
}

void Buzzer_Off(){
   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    
}
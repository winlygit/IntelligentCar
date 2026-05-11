#include "main.h"

#define Buzzer_Autoreload  1000

void Buzzer_On(){
    __HAL_TIM_SetAutoreload(TIM3,Buzzer_Autoreload);
    __HAL_TIM_SetCompare(TIM3,Buzzer_Autoreload/2);
}

void Buzzer_Off(){
    __HAL_TIM_SetCompare(TIM3,0);
    
}
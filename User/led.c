#include "main.h"



void LED_On(void){
    HAL_GPIO_WritePin(GPIOA ,GPIO_PIN_13,GPIO_PIN_SET);
}


void LED_Off(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_13,GPIO_PIN_RESET);
}


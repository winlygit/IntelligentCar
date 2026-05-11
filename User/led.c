#include "main.h"



void LED_On(){
    HAL_GPIO_WritePin(GPIOA ,GPIO_PIN_13,GPIO_PIN_SET);
}


void LED_OFF(){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_13,GPIO_PIN_RESET);
}


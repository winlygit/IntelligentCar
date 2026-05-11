#ifndef __UART_H
#define __UART_H


#include "main.h"

//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;





//void U1_printf (const uint8_t* pData);
//void U2_printf (const uint8_t* pData);
void U3_printf (const uint8_t* pData);
void uart_init(void);







#endif

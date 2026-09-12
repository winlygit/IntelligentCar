
/*
******************************************************************************
        本文件用于存放串口驱动函数
        以及串口中断处理
        
        
******************************************************************************
*/


#include "main.h"
#include "uart.h"
#include "global.h"
#include "stdio.h"





void U1_printf (const uint8_t* pData ) //向串口1发送数据
{   
   if (pData == NULL){
       return;
   }
   
   uint16_t len = 0;
   while (pData[len] != '\0')
   {
       len++;
   }
   HAL_UART_Transmit(&huart1,pData,len,HAL_MAX_DELAY);


}

void U2_printf (const uint8_t* pData ) //向串口2发送数据
{   
   if (pData == NULL){
       return;
   }
   
   uint16_t len = 0;
   while (pData[len] != '\0')
   {
       len++;
   }
   HAL_UART_Transmit(&huart2,pData,len,HAL_MAX_DELAY);


}


void U3_printf (const uint8_t* pData ) //向串口3发送数据
{   
    if (pData == NULL){
        return;
    }
    
    uint16_t len = 0;
    while (pData[len] != '\0')
    {
        len++;
    }

    
    HAL_UART_Transmit(&huart3,pData,len,HAL_MAX_DELAY);
    uint8_t a[] = "STM32:";
    HAL_UART_Transmit(&huart1,a,6,HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart1,pData,len,HAL_MAX_DELAY);


}

void U4_printf (const uint8_t* pData ) //向串口4发送数据
{   
    if (pData == NULL){
        return;
    }
    
    uint16_t len = 0;
    while (pData[len] != '\0')
    {
        len++;
    }
    HAL_UART_Transmit(&huart4,pData,len,HAL_MAX_DELAY);


}

// void U5_printf (const uint8_t* pData ) //向串口5发送数据
// {   
//     if (pData == NULL){
//         return;
//     }
    
//     uint16_t len = 0;
//     while (pData[len] != '\0')
//     {
//         len++;
//     }
//     HAL_UART_Transmit(&huart5,pData,len,HAL_MAX_DELAY);


// }




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart == &huart3){
        if(value_Phone == '@'){
            uart_index_Phone = 0;
            ifrxstart_Phone = 1;
        }
        
        if(ifrxstart_Phone ==1) RxData_Phone[uart_index_Phone++] = value_Phone;

        if(value_Phone == '#') rxcplt_flag_Phone = 1;

        HAL_UART_Receive_IT(&huart3,&value_Phone,1);

    }else if(huart == &huart2){
        if(value_Openmv == '@'){
            uart_index_Openmv = 0;
            ifrxstart_Openmv = 1;
        }
        
        if(ifrxstart_Openmv ==1) RxData_Openmv[uart_index_Openmv++] = value_Openmv;

        
        if(value_Openmv == '#') rxcplt_flag_Openmv = 1;
        
        HAL_UART_Receive_IT(&huart2,&value_Openmv,1);
    }
    
    
}

void uart_init(void){
    
    HAL_UART_Receive_IT(&huart3,&value_Phone,1);
    HAL_UART_Receive_IT(&huart2,&value_Openmv,1);
}
 



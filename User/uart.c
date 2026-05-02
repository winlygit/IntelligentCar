
/*
******************************************************************************
        本文件用于存放串口驱动函数
        以及串口中断处理
        
        
******************************************************************************
*/


#include "main.h"
#include "uart.h"
#include "global.h"





//void U1_printf (const uint8_t* pData ) //向串口1发送数据
//{   
//    if (pData == NULL){
//        return;
//    }
//    
//    uint16_t len = 0;
//    while (pData[len] != '\0')
//    {
//        len++;
//    }
//    HAL_UART_Transmit_IT(&huart1,pData,len);


//}

//void U2_printf (const uint8_t* pData ) //向串口2发送数据
//{   
//    if (pData == NULL){
//        return;
//    }
//    
//    uint16_t len = 0;
//    while (pData[len] != '\0')
//    {
//        len++;
//    }
//    HAL_UART_Transmit_DMA(&huart2,pData,len);


//}


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


}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    
    if(value == '@'){
    uart_index = 0;
    ifrxstart = 1;
    }
    
    if(ifrxstart ==1){
    RxData[uart_index++] = value;

    }
    
    if(value == '#'){
        
        rxcplt_flag = 1;
            
    }
    
    HAL_UART_Receive_IT(&huart3,&value,1);
    
}
 



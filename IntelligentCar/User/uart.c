
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



void USART3_IRQHandler(void)//读取上位机数据
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}
 

#include "stm32f10x_it.h"
#include "bsp_usart.h"

/*
 *串口1中断服务函数
 */
void USART1_IRQHandler(void)
{
  uint8_t ucTemp;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(USART1);
    USART_SendData(USART1,ucTemp);    
	}	 
}

/*
 *串口2中断服务函数
 */
void USART2_IRQHandler(void)
{
  uint8_t ucTemp;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(USART2);
    USART_SendData(USART2,ucTemp);    
	}	 
}


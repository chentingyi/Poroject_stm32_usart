#include "stm32f10x.h"
#include "bsp_usart.h"

/*
 * @brief  ������
 */
int main(void)
{	
  USART_Config(USART1,115200);   
	USART_Config(USART2,9600);

	Usart_SendString( USART1,"����һ������1  �жϽ��ջ���ʵ��\n");
	printf("����һ������2  �жϽ��ջ���ʵ��\n");
	
  while(1)
	{	
			Send_Float("t1",1.123456);        
			Send_Int("n0",-999);
			Send_Str("t3","��ã����磡");   
	}	
}




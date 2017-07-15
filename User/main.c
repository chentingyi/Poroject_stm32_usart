#include "stm32f10x.h"
#include "bsp_usart.h"

/*
 * @brief  主函数
 */
int main(void)
{	
  USART_Config(USART1,115200);   
	USART_Config(USART2,9600);

	Usart_SendString( USART1,"这是一个串口1  中断接收回显实验\n");
	printf("这是一个串口2  中断接收回显实验\n");
	
  while(1)
	{	
			Send_Float("t1",1.123456);        
			Send_Int("n0",-999);
			Send_Str("t3","你好，世界！");   
	}	
}




#include "bsp_usart.h"

/*
 *配置嵌套向量中断控制器NVIC
 */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
}

/*
 *USART GPIO 配置,工作参数配置，本工程只能初始化串口 1 和串口 2
 */
void USART_Config(USART_TypeDef* USARTx,u32 BTL)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);// 打开串口GPIO的时钟

	if(USARTx == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// 打开串口外设的时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	// 打开串口外设的时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	}
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	if(USARTx == USART1)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	}
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	USART_InitStructure.USART_BaudRate = BTL;// 配置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	// 配置 针数据字长
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	// 配置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No ;	// 配置校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 配置硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 配置工作模式，收发一起
	USART_Init(USARTx, &USART_InitStructure);	// 完成串口的初始化配置
	
	NVIC_Configuration();//串口中断优先级配置
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//使能串口接收中断
	USART_Cmd(USARTx, ENABLE);//使能串口    
}


/*
 *发送一个字节
 */
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	USART_SendData(pUSARTx,ch);	/* 发送一个字节数据到USART */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	/* 等待发送数据寄存器为空 */	
}


/*
 *发送8位的数组
 */
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    Usart_SendByte(pUSARTx,array[i]);		    /* 发送一个字节数据到USART */ 
  }

	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);	/* 等待发送完成 */
}

/*
 *发送字符串
 */
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)/*等待发送完成*/
  {}
}

/*
 *发送一个16位数 
 */
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	temp_h = (ch&0XFF00)>>8;/*取出高八位 */
	temp_l = ch&0XFF;/*取出低八位 */
	USART_SendData(pUSARTx,temp_h);/* 发送高八位 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(pUSARTx,temp_l);/* 发送低八位 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*
 *重定向c库函数printf到串口，重定向后可使用printf函数
 */
int fputc(int ch, FILE *f)
{
		USART_SendData(USART2, (uint8_t) ch);/* 发送一个字节数据到串口 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);/* 等待发送完毕 */	
		return (ch);
}

/*
 *重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
 */
int fgetc(FILE *f)
{
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);/* 等待串口输入数据 */
		return (int)USART_ReceiveData(USART2);
}

/********以下是串口屏函数*********/
void jieshu(void)
{
	Usart_SendByte(USART2,0XFF);   
	Usart_SendByte(USART2,0XFF);
	Usart_SendByte(USART2,0XFF);
}

void Send_Int(char *Name,int num)         
{        
		printf("%s.val=%d",Name,num);  
		jieshu();
}

void Send_Float(char *Name,float fl)
{
		printf("%s.txt=\"%.6f Hz\"",Name,fl);  
		jieshu();  
}

void Send_Str(char *Name,char *str)
{
		printf("%s.txt=\"%s\"",Name,str);  
		jieshu();  
}

/***********************************/


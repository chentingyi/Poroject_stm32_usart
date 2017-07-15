#include "bsp_usart.h"

/*
 *����Ƕ�������жϿ�����NVIC
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
 *USART GPIO ����,�����������ã�������ֻ�ܳ�ʼ������ 1 �ʹ��� 2
 */
void USART_Config(USART_TypeDef* USARTx,u32 BTL)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);// �򿪴���GPIO��ʱ��

	if(USARTx == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// �򿪴��������ʱ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	// �򿪴��������ʱ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	}
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	USART_InitStructure.USART_BaudRate = BTL;// ���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	// ���� �������ֳ�
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	// ����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No ;	// ����У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// ����Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// ���ù���ģʽ���շ�һ��
	USART_Init(USARTx, &USART_InitStructure);	// ��ɴ��ڵĳ�ʼ������
	
	NVIC_Configuration();//�����ж����ȼ�����
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//ʹ�ܴ��ڽ����ж�
	USART_Cmd(USARTx, ENABLE);//ʹ�ܴ���    
}


/*
 *����һ���ֽ�
 */
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	USART_SendData(pUSARTx,ch);	/* ����һ���ֽ����ݵ�USART */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	/* �ȴ��������ݼĴ���Ϊ�� */	
}


/*
 *����8λ������
 */
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    Usart_SendByte(pUSARTx,array[i]);		    /* ����һ���ֽ����ݵ�USART */ 
  }

	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);	/* �ȴ�������� */
}

/*
 *�����ַ���
 */
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)/*�ȴ��������*/
  {}
}

/*
 *����һ��16λ�� 
 */
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	temp_h = (ch&0XFF00)>>8;/*ȡ���߰�λ */
	temp_l = ch&0XFF;/*ȡ���Ͱ�λ */
	USART_SendData(pUSARTx,temp_h);/* ���͸߰�λ */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(pUSARTx,temp_l);/* ���͵Ͱ�λ */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*
 *�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
 */
int fputc(int ch, FILE *f)
{
		USART_SendData(USART2, (uint8_t) ch);/* ����һ���ֽ����ݵ����� */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);/* �ȴ�������� */	
		return (ch);
}

/*
 *�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
 */
int fgetc(FILE *f)
{
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);/* �ȴ������������� */
		return (int)USART_ReceiveData(USART2);
}

/********�����Ǵ���������*********/
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


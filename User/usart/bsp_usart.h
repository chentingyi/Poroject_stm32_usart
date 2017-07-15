#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"
#include <stdio.h>

void USART_Config(USART_TypeDef* USARTx,u32 btl);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

void Send_Int(char *Name,int num);
void Send_Float(char *Name,float num);
void Send_Str(char *Name,char *str);
void jieshu(void);

#endif

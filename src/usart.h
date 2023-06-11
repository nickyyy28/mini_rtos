#ifndef __USART_H
#define __USART_H

#include "super.h"
#include <stdio.h>

typedef struct {
	volatile uint32_t USART_SR;
	volatile uint32_t USART_DR;
	volatile uint32_t USART_BRR;
	volatile uint32_t USART_CR1;
	volatile uint32_t USART_CR2;
	volatile uint32_t USART_CR3;
	volatile uint32_t USART_GTPR;
}USART_TypeDef;

void usart1_init(void);
void put_char(char ch);
void usart1_send_string(char *dst, unsigned int len);
void send_addr(void *addr);

#endif

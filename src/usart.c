#include "usart.h"

void usart1_init(void)
{
	/*//ENABLE GPIOA & USART1
	*(unsigned int *)(APB2_ENR) |= (1<<2);
	*(unsigned int *)(APB2_ENR) |= (1<<14);
	
	//配置PA9 USART1_TX,PA10 USART1_RX
	*(unsigned int *)(GPIOA + 0x04) &= ~(3<<4);		//清零
	*(unsigned int *)(GPIOA + 0x04) |= (01<<4);		// PA9  OutPut 10MHz
	*(unsigned int *)(GPIOA + 0x04) &= ~(3<<8);		// PA10	Input
	
	*(unsigned int *)(GPIOA + 0x04) &= ~(3<<6);		//清零
	*(unsigned int *)(GPIOA + 0x04) |= (2<<6);		//PA9	复用推挽输出	
	*(unsigned int *)(GPIOA + 0x04) &= ~(3<<10);	//清零
	*(unsigned int *)(GPIOA + 0x04) |= (01<<10);	//PA10	浮空输入
	
	//配置BoundRate
	USART_TypeDef *usart1 = (USART_TypeDef *)USART1;
	
	usart1->USART_BRR = (4 << 4) | (5); 
	
	usart1->USART_CR1 |= (1<<13);	//enable usart
	usart1->USART_CR1 &= ~(1<<12);	//1 start bit, 8 data bit
	usart1->USART_CR1 &= ~(1<<10);	//不校验
	
	usart1->USART_CR1 |= (1 << 3) | (1 << 2);	//enable transmit & receive
	
	usart1->USART_CR2 &= ~(3 << 12);//1 stop bit*/
	
	USART_TypeDef *usart1 = (USART_TypeDef *)0x40013800;
	volatile unsigned int *pReg;
	/* 使能GPIOA/USART1模块 */
	/* RCC_APB2ENR */
	pReg = (volatile unsigned int *)(0x40021000 + 0x18);
	*pReg |= (1<<2) | (1<<14);
	
	/* 配置引脚功能: PA9(USART1_TX), PA10(USART1_RX) 
	 * GPIOA_CRH = 0x40010800 + 0x04
	 */
	pReg = (volatile unsigned int *)(0x40010800 + 0x04);
	
	/* PA9(USART1_TX) */
	*pReg &= ~((3<<4) | (3<<6));
	*pReg |= (1<<4) | (2<<6);  /* Output mode, max speed 10 MHz; Alternate function output Push-pull */

	/* PA10(USART1_RX) */
	*pReg &= ~((3<<8) | (3<<10));
	*pReg |= (0<<8) | (1<<10);  /* Input mode (reset state); Floating input (reset state) */
	
	/* 设置波特率
	 * 115200 = 8000000/16/USARTDIV
	 * USARTDIV = 4.34
	 * DIV_Mantissa = 4
	 * DIV_Fraction / 16 = 0.34
	 * DIV_Fraction = 16*0.34 = 5
	 * 真实波特率:
	 * DIV_Fraction / 16 = 5/16=0.3125
	 * USARTDIV = DIV_Mantissa + DIV_Fraction / 16 = 4.3125
	 * baudrate = 8000000/16/4.3125 = 115942
 	 */
#define DIV_Mantissa 4
#define DIV_Fraction 5
	usart1->USART_BRR = (DIV_Mantissa<<4) | (DIV_Fraction);
	
	/* 设置数据格式: 8n1 */
	usart1->USART_CR1 = (1<<13) | (0<<12) | (0<<10) | (1<<3) | (1<<2);	
	usart1->USART_CR2 &= ~(3<<12);
	
	/* 使能USART1 */
}

void put_char(char ch)
{
	USART_TypeDef *usart1 = (USART_TypeDef *)USART1;
	while(!(usart1->USART_SR & (1 << 7)));
	usart1->USART_DR = ch;
	
}

void usart1_send_string(char *dst, unsigned int len)
{
	unsigned int a = 0;
	while(a != len)
	{
		put_char(*(dst+a));
		a++;
	}
}

void send_addr(void *addr)
{
	char temp[12] = {0};
	sprintf(temp, "0x%p",addr);
	temp[10] = '\r';
	temp[11] = '\n';
	usart1_send_string(temp, 12);
}

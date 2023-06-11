#include "usart.h"
#include "exception.h"
#include "systick.h"
#include "led.h"
#include "task.h"


void SysTickInit(void)
{
	SysTick_Type *SysTick = (SysTick_Type *)SysTick_BASE;
	/* 1. set period : 1S  */
	/* 2. set Load */
	SysTick->VAL  = SYSTICK_FRE;
	SysTick->LOAD = SYSTICK_FRE;
	
	/* 3. select clk source, enable systick, enable exception 
	 *  BIT[2] : clk source, process clk
	 *  BIT[1] : enable exception
	 *  BIT[0] : enable systick
	 */
	SysTick->CTRL = (1<<2) | (1<<1) | (1<<0);
}
char sys_tick_buffer[30] = {0};
#include <stdio.h>
void SysTick_Handler(void)
{
	SCB_Type * SCB = (SCB_Type *)SCB_BASE_ADDR;
	/* clear exception status */
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
	
	//sprintf(sys_tick_buffer, "systick coming, lr=%x\r\n", 0xFFFFFFF9);
	//usart1_send_string(sys_tick_buffer, 30);
	
	//os调度
	os_schedule();
	
}

#include "usart.h"
#include "exception.h"

void HardFault_Handler(void)
{ 
	usart1_send_string("hard fault comming\r\n", 20);
	while(1);
}

void UsageFault_Handler(unsigned int *stack)
{
	usart1_send_string("usage fault comming\r\n", 21);
	
	SCB_Type * SCB = (SCB_Type *)SCB_BASE_ADDR;

	/* 1. clear usage fault */
	SCB->CFSR = SCB->CFSR;
	
	/* 2. set return address in sp, piont to next instruction */
	stack[6] += 4;
	while(1);
}

__weak void func(void)
{
	
}

void UsageFaultInit(void)
{
	SCB_Type * SCB = (SCB_Type *)SCB_BASE_ADDR;
	SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk);
}

void SVC_Handler(void)
{
	volatile int buff[20] = {0};
	usart1_send_string("svc fault comming\r\n", 19);
}
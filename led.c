#include "super.h"
	
void LedInit(void)
{	
	unsigned int *pReg;
	
	/* 使能GPIOC */
	//pReg = (unsigned int *)(RCC_BASE + 0x18);
	RCC->APB2ENR |= (1<<4);
	//*pReg |= (1<<4);
	
	/* 设置GPIOC13为输出引脚 */
	pReg = (unsigned int *)(GPIOC + 0x4);
	*pReg |= (1<<20);

	
}

void LedCtrl(int on)
{
	unsigned int *pReg;
	pReg = (unsigned int *)(GPIOC + 0x0C);
	
	if (on)
	{
		/* 设置GPIOC13输出0 */
		*pReg &= ~(1<<13);
	}
	else
	{
		/* 设置GPIOC13输出1 */
		*pReg |= (1<<13);
	}	
}

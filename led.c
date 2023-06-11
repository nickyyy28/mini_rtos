#include "super.h"
	
void LedInit(void)
{	
	unsigned int *pReg;
	
	/* ʹ��GPIOC */
	//pReg = (unsigned int *)(RCC_BASE + 0x18);
	RCC->APB2ENR |= (1<<4);
	//*pReg |= (1<<4);
	
	/* ����GPIOC13Ϊ������� */
	pReg = (unsigned int *)(GPIOC + 0x4);
	*pReg |= (1<<20);

	
}

void LedCtrl(int on)
{
	unsigned int *pReg;
	pReg = (unsigned int *)(GPIOC + 0x0C);
	
	if (on)
	{
		/* ����GPIOC13���0 */
		*pReg &= ~(1<<13);
	}
	else
	{
		/* ����GPIOC13���1 */
		*pReg |= (1<<13);
	}	
}

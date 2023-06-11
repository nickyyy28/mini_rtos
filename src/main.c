#include "super.h"
#include "usart.h"
#include "led.h"
#include "task.h"

int var_1 = 0;
void task1(void *arg);
void task2(void *arg);
void delay(int d)
{
	while(d--) __nop();
}

void func1(int a, int b)
{
	var_1 = a + b;
}

void mymemcpy(void *dst, void *src, unsigned int len)
{
//	size_t a;
//	while(len --)
//	{
//		*((unsigned char *)dst + len) = *((unsigned char *)src + len);
//	}
	unsigned char *pcDest = dst;
	unsigned char *pcSrc  = src;
	
	while (len --)
	{
		*pcDest = *pcSrc;
		pcSrc++;
		pcDest++;
	}
}

void mymemset(void *dst, unsigned char value, unsigned int len)
{
	unsigned char *pcDest = dst;
	
	while (len --)
	{
		*pcDest = value;
		pcDest++;
	}
}

void SystemInit(void)
{
	extern int * Image$$ER_IROM1$$Base;
	extern int * Image$$ER_IROM1$$Length;
	extern int * Load$$ER_IROM1$$Base;
	extern int * Image$$RW_IRAM1$$Base;
	extern int * Image$$RW_IRAM1$$Length;
	extern int * Load$$RW_IRAM1$$Base;
	extern int * Image$$RW_IRAM1$$ZI$$Base;
	extern int * Image$$RW_IRAM1$$ZI$$Length;	
	
	/* text relocate */
	if (&Image$$ER_IROM1$$Base != &Load$$ER_IROM1$$Base)
		memcpy(&Image$$ER_IROM1$$Base, &Load$$ER_IROM1$$Base, &Image$$ER_IROM1$$Length);
	
	/* data relocate */
	if (&Image$$RW_IRAM1$$Base != &Load$$RW_IRAM1$$Base)
		memcpy(&Image$$RW_IRAM1$$Base, &Load$$RW_IRAM1$$Base, &Image$$RW_IRAM1$$Length);
	
	/* bss clear */
	memset(&Image$$RW_IRAM1$$ZI$$Base, 0, &Image$$RW_IRAM1$$ZI$$Length);
}

char c = 'F';
const char c2 = 'B';

int g_a[20] = {4,5,9};
int g_b;

int main(void)
{
	usart1_init();
	//enable GPIOC
	*(APB2_ENR) |= (1<<4);
	
	unsigned int *led = (unsigned int *)(GPIOC + 0x0C);
	unsigned int *led_conf = (unsigned int *)(GPIOC + 0x04);
	
	//config GPIOC
	
	//set gpio speed as 10MHz
	*led_conf |= (01 << 20);
	
	char temp[20] = {0};
	sprintf(temp, "g_a[0] = %d\r\n", 10);
	usart1_send_string(temp,20);
	
	while(1)
	{
		//delay 1000s
		delay(1000000);
		*led &= ~(1 << 13);
		//usart1_send_string(temp,20);
		send_addr(&c);
		send_addr((void *)&c2);
		delay(1000000);
		*led |= (1 << 13);
		func1(1, 2);
	}
	
	
}

volatile int a = 123;

int add(int *a, int *b)
{
	volatile int tmp = *a;
	tmp += *b;
	register int aaaa __asm("r4");
	aaaa = *a + *b;
	return tmp;
}

int func(int v1, int v2,int v3,int v4,int v5,int v6,int v7,int v8,int v9,int v10,int v11,int v12,int v13,int v14,int v15,int v16)
{
	volatile int tmp;
	tmp += v1;
	tmp += v2;
	tmp += v3;
	tmp += v4;
	tmp += v5;
	tmp += v6;
	tmp += v7;
	tmp += v8;
	tmp += v9;
	tmp += v10;
	tmp += v11;
	tmp += v12;
	tmp += v13;
	tmp += v14;
	tmp += v15;
	tmp += v16;
	return tmp;
}

volatile static __attribute__((__aligned(4))) uint8_t task1_stack[512];
volatile static __attribute__((__aligned(4))) uint8_t task2_stack[512];

int mymain()
{
	create_task("task1", task1, "task1", PriorityNormal, (uint8_t*)task1_stack, 256);
	create_task("task2", task2, "task2", PriorityNormal, (uint8_t*)task2_stack, 512);

	os_start_schedule();
	return 0;
}

float task1_usage = 0;
void task1(void *arg)
{
	volatile int a = 0;
	char buffer[30] = "this is task1 num:   \r\n";
	uint8_t flag = 0;
	int tmp_var = 0;
	while(1) {
		//do something
		buffer[19] = (char)(tmp_var / 100 + 48);
		buffer[20] = (char)((tmp_var % 100) / 10 + 48);
		buffer[21] = (char)(tmp_var % 10 + 48);
		usart1_send_string(buffer, 23);
		flag = ! flag;
		tmp_var++;
		if (tmp_var > 100) {
			tmp_var = 0;
		}
		task_delay(1);
		task1_usage = stack_usage();
		delay(1000000);
	}
}

void task2(void *arg)
{
	volatile int a = 0;
	char buffer[30] = "this is task2 num:   \r\n";
	int tmp_num = 0;
	while(1) {
		//do something
		//切换任务栈
		buffer[19] = (char)(tmp_num / 100 + 48);
		buffer[20] = (char)((tmp_num % 100) / 10 + 48);
		buffer[21] = (char)(tmp_num % 10 + 48);
		usart1_send_string(buffer, 23);
		tmp_num++;
		if (tmp_num > 100) {
			tmp_num = 0;
		}
		task_delay(1);
		delay(1000000);
	}
}

void switch_task(void)
{
	//save current task stack
	
	//load next task stack
}


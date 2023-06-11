#include "task.h"
#include <stdio.h>
#include "cmsis_armcc.h"
#include "usart.h"

THREAD_LOCAL TASK_NODE* cur_task = NULL;
TASK_NODE *idle_task = NULL;
static TASK_LIST os_task_list = {.tasks = {0}, .size = 0, .highest_node = NULL, .lowest_node = NULL};
static uint8_t is_os_started = 0;

static TASK_TCB* list_add_task(const char *task_name, TASK_FUNCTION func, void* param, TASK_PRIORITY priority, uint8_t *stack, uint32_t stack_size);
static void task_tcb_init(TASK_TCB* tcb, const char *task_name, TASK_FUNCTION func, void* param, TASK_PRIORITY priority, uint8_t *stack, uint32_t stack_size);
static __asm void start_task(int stack, int lr_);
static __asm void resume_task(int stack, int lr);

TASK_TCB* create_task(const char *task_name, TASK_FUNCTION func, void* param, TASK_PRIORITY priority, uint8_t *stack, uint32_t stack_size)
{
	return list_add_task(task_name, func, param, priority, stack, stack_size);
}

static TASK_TCB* list_add_task(const char *task_name, TASK_FUNCTION func, void* param, TASK_PRIORITY priority, uint8_t *stack, uint32_t stack_size)
{
	TASK_TCB *tcb = NULL;
	TASK_NODE *node = NULL;
	if (os_task_list.size >= MAX_TASK_COUNT) {
		return tcb;
	}
	if (os_task_list.size == 0) {
		task_tcb_init(&os_task_list.tasks[os_task_list.size].task, task_name, func, param, priority, stack, stack_size);
		tcb = &os_task_list.tasks[os_task_list.size].task;
		os_task_list.highest_node = &os_task_list.tasks[os_task_list.size];
		os_task_list.lowest_node = &os_task_list.tasks[os_task_list.size];
		os_task_list.tasks[os_task_list.size].next = NULL;
		os_task_list.tasks[os_task_list.size].pre = NULL;
		node = &os_task_list.tasks[os_task_list.size];
		os_task_list.size ++;
	} else {
		task_tcb_init(&os_task_list.tasks[os_task_list.size].task, task_name, func, param, priority, stack, stack_size);
		tcb = &os_task_list.tasks[os_task_list.size].task;
		node = &os_task_list.tasks[os_task_list.size];
		os_task_list.size ++;
		if (os_task_list.highest_node == os_task_list.lowest_node) {
			if (priority > os_task_list.highest_node->task.priority) {
				os_task_list.lowest_node->next = node;
				os_task_list.highest_node = node;
				node->pre = os_task_list.lowest_node;
			} else {
				os_task_list.highest_node->pre = node;
				os_task_list.lowest_node = node;
				node->next = os_task_list.highest_node;
			}
		} else {
			if (node->task.priority <= os_task_list.lowest_node->task.priority) {
				os_task_list.lowest_node->pre = node;
				node->next = os_task_list.lowest_node;
				node->pre = NULL;
				os_task_list.lowest_node = node;
			} else if (node->task.priority >= os_task_list.highest_node->task.priority) {
				os_task_list.highest_node->next = node;
				node->pre = os_task_list.highest_node;
				node->next = NULL;
				os_task_list.highest_node = node;
			} else {
				TASK_NODE *tmp = os_task_list.lowest_node;
				while (tmp->next->task.priority <= node->task.priority) {
					tmp = tmp->next;
				}
				node->next = tmp->next;
				tmp->next->pre = node;
				tmp->next = node;
				node->pre = tmp;
			}
		}
	}
	node->status = TASK_READY;

	/* 第一次进入函数没有编译器和cpu帮助保存恢复寄存器,所以需要伪造现场 */
	int *stack_top = (int *)(stack + stack_size);
	tcb->stack_top = stack_top;
	stack_top -= 16; //需要保存16个寄存器 PSR, PC, LR, R12, R3, R2, R1, R0, R11, R10, R9, R8, R7, R6, R5, R4

	/*R4-R11*/
	stack_top[0] = 0; //R4
	stack_top[1] = 0; //R5
	stack_top[2] = 0; //R6
	stack_top[3] = 0; //R7
	stack_top[4] = 0; //R8
	stack_top[5] = 0; //R9
	stack_top[6] = 0; //R10
	stack_top[7] = 0; //R11

	/*R0-R3*/
	stack_top[8] = (int)param; //R0 函数参数
	stack_top[9] = 0; //R1
	stack_top[10] = 0; //R2
	stack_top[11] = 0; //R3

	/*R12, LR, PC, PSR*/
	stack_top[12] = 0; //R12
	stack_top[13] = 0; //LR 没有返回地址,所以设置为0
	stack_top[14] = (int)func; //PC 设为函数入口地址
	// stack_top[15] = 0x01000000; //PSR
	stack_top[15] = (1 << 24); //PSR 将Thumb位设置为1,使用Thumb指令集

	tcb->cur_stack = (uint32_t*)stack_top;	//保存当前栈指针
	tcb->isFirstRun = 1;
	return tcb;
}


static void task_tcb_init(TASK_TCB* tcb, const char *task_name, TASK_FUNCTION func, void* param, TASK_PRIORITY priority, uint8_t *stack, uint32_t stack_size)
{
	tcb->task_name = task_name;
	tcb->f = func;
	tcb->param = param;
	tcb->priority = priority;
	tcb->stack_base = stack;
	tcb->stack_size = stack_size;
}

uint8_t is_started(void)
{
	return is_os_started;
}

uint8_t default_stack[128] = {0};
__weak void default_task(void *p)
{
	while(1) {
		usart1_send_string("this is default task\r\n", 23);
		task_delay(1);
		delay(1000000);
	}
}

void os_start_schedule(void)
{
	is_os_started = 1;
	create_task("default", default_task, NULL, PriorityIDLE, default_stack, 128);
	idle_task = &os_task_list.tasks[os_task_list.size - 1];
	while (1);
}

float stack_usage()
{
	if (cur_task == NULL) return 0;
	float usage = ((char*)cur_task->task.stack_top - (char*)cur_task->task.cur_stack );
	usage /= cur_task->task.stack_size * 1.0;
	return usage;
}

TASK_NODE *delay_node = NULL, *init_node = NULL, *next_node = NULL;
void os_schedule(void)
{
	if (is_started()) {
		//遍历任务链表, 将有延时的任务的延时时间减1
		delay_node = os_task_list.highest_node;
		while (delay_node != NULL) {
			if (delay_node->status == TASK_DELAY) {
				delay_node->delay_tick --;
				if (delay_node->delay_tick == 0) {
					delay_node->status = TASK_READY;
				}
			}
			delay_node = delay_node->pre;
		}

		//有两个可能, 系统没有开始调度,cur_task为NULL, 系统已经开始调度,cur_task不为NULL
		if (cur_task == NULL) {
			//系统第一次开始调度, 从最高优先级的任务开始调度
			init_node = os_task_list.highest_node;
			//找到最高优先级且状态为TASK_READY的任务
			while(init_node != NULL && init_node->status != TASK_READY) {
				init_node = init_node->pre;
			}
			if (init_node != NULL) {
				init_node->status = TASK_EXEC;
				cur_task = init_node;
				//由于系统第一次调度,所以可以直接从函数入口地址开始运行
				usart1_send_string("os_start_task\r\n", 15);
				cur_task->task.isFirstRun = 0;
				start_task((int)cur_task->task.cur_stack, 0xFFFFFFF9);
			}
		} else {
			//系统不是第一次开始调度, 需要找到下一个调度任务
			next_node = os_task_list.highest_node;
			//找到最高优先级且状态为TASK_READY的任务
			while(next_node != NULL && next_node->status != TASK_READY) {
				next_node = next_node->pre;
			}
			if (next_node != NULL) {
				cur_task = next_node;
				if (cur_task->task.isFirstRun) {
					//切换的任务为第一次进入调度
					cur_task->status = TASK_EXEC;
					cur_task->task.isFirstRun = 0;
					//由于任务第一次运行, 直接从函数入口地址开始运行
					usart1_send_string("start_task\r\n", 12);
					start_task((int)cur_task->task.cur_stack, 0xFFFFFFF9);
				} else {
					//切换的任务不是第一次进入调度, 需要从上一次中断的地方开始运行
					cur_task->status = TASK_EXEC;
					usart1_send_string("resume_task\r\n", 13);
					resume_task((int)cur_task->task.cur_stack, 0xFFFFFFF9);
				}
			} else {
				//找不到任何可以调度的任务, 恢复到IDLEtask
				cur_task = idle_task;
				cur_task->status = TASK_EXEC;
				resume_task((int)cur_task->task.cur_stack, 0xFFFFFFF9);
			}
		}
		
		//遍历任务链表, 将有延时的任务的延时时间减1
		/*delay_node = os_task_list.highest_node;
		while (delay_node != NULL) {
			if (delay_node->status == TASK_DELAY) {
				delay_node->delay_tick --;
				if (delay_node->delay_tick == 0) {
					delay_node->status = TASK_READY;
				}
			}
			delay_node = delay_node->pre;
		}*/
	} else {
		return;
	}
}

#if defined ( __CC_ARM )



static __asm void start_task(int stack, int lr_)
{
	//R0是栈指针, R1是返回地址
	//从任务栈中将R4-R11读出来写入寄存器
	LDMIA R0!, {R4-R11}

	//更新SP
	MSR MSP, R0

	//触发硬件中断返回
	BX R1
}

static __asm void resume_task(int stack, int lr)
{
	//恢复现场
	LDMIA R0!, {R4-R11}
	//更新SP
	MSR MSP, R0
	//触发硬件中断返回
	BX R1
}

// static int *int_var = NULL;
void __asm save_stack_frame(void)
{
	//保存当前栈的数据
	
	//判断cur_task是不是null
	IMPORT |cur_task|
	LDR R0, =|cur_task|
	LDR R0, [R0, #0]
	CMP R0, #0
	BEQ DO_NOT_SAVE_STACK1
				
SAVE_OLD_STACK1
	STMDB SP!, {R4-R11}
	LDR R0, =|cur_task|
	LDR R0, [R0, #0]
	STR SP, [R0, #0x1c]
				
DO_NOT_SAVE_STACK1
	PUSH {LR}
	POP  {PC}
	
// 	BEQ	save_stack_quit
	
// save_stack_quit:
	
	// CMP |cur_task|, #0
}

void __asm push_r4_r11(void)
{
	//保存当前栈的数据
	PUSH {R4-R11}
	PUSH {LR}
	
	POP {PC}
}

void save_stack(void)
{
	if (cur_task == NULL) {
		return;
	}
	//保存当前栈的数据
	push_r4_r11();
	// cur_task->task.cur_stack = 
	register uint32_t __regMainStackPointer     __ASM("sp");
	cur_task->task.cur_stack = (uint32_t *)__regMainStackPointer;
}

#elif defined ( __GNUC__ )

#elif defined ( __ICCARM__ )

#elif defined ( __TASKING__ )

#endif

TASK_NODE* get_current_task(void)
{
	return cur_task;
}

void yeild(void)
{
	cur_task->status = TASK_TERM;
}

void task_delay(uint32_t ms)
{
	cur_task->status = TASK_DELAY;
	cur_task->delay_tick = ms + 1;
}
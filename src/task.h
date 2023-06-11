#ifndef __TASK_H
#define __TASK_H

#include <stdint.h>

#define THREAD_LOCAL
#define MAX_TASK_COUNT	20

typedef void (*TASK_FUNCTION)(void*);

typedef enum{
	None = 0x0,
	PriorityIDLE,
	PriorityNormal,
	PriorityHigh,
	PriorityRealTime
}TASK_PRIORITY;


typedef struct{
	TASK_FUNCTION f;		//任务入口函数
	void* param;			//任务参数
	TASK_PRIORITY priority; //任务优先级
	uint8_t *stack_base; 	//任务栈基地址
	const char *task_name;	//任务名
	uint32_t stack_size;	//任务栈大小
	uint32_t *stack_top;	//任务栈顶
	uint32_t *cur_stack;	//当前栈指针
	uint8_t isFirstRun;		//是否第一次运行
}TASK_TCB;

typedef enum{
	TASK_DISABLED = 0x0,	//禁止调度
	TASK_READY = 0x1,		//准备好调度
	TASK_EXEC,				//正在运行
	TASK_TERM,				//挂起
	TASK_DELAY,				//延时
}TASK_STATUS;

typedef struct TASK_NODE{
	TASK_TCB task;			//任务控制块
	TASK_STATUS status;		//任务状态
	uint32_t delay_tick;	//延时的tick
	struct TASK_NODE *pre;	//上一个任务节点 低优先级
	struct TASK_NODE *next;	//下一个任务节点 高优先级
}TASK_NODE;

typedef struct{
	TASK_NODE tasks[MAX_TASK_COUNT];	//所有的任务节点数组
	uint32_t size;						//当前任务数量
	TASK_NODE *highest_node;			//最高优先级任务
	TASK_NODE *lowest_node;				//最低优先级任务
}TASK_LIST;

/**
 * @brief 创建一个任务
 * 
 * @param task_name 	任务名
 * @param func 			任务入口函数
 * @param param 		任务参数
 * @param priority 		优先级
 * @param stack 		任务栈
 * @param stack_size 	栈大小
 * @return TASK_TCB* 	任务控制块
 */
TASK_TCB* create_task(const char *task_name, TASK_FUNCTION func, void* param, TASK_PRIORITY priority, uint8_t *stack, uint32_t stack_size);

/**
 * @brief os开始调度
 * 
 */
void os_start_schedule(void);

/**
 * @brief os调度函数
 * 
 * @param lr_ 
 */
void os_schedule(void);

/**
 * @brief os是否开始调度
 * 
 * @return uint8_t 
 */
uint8_t is_started(void);

/**
 * @brief 获取当前任务
 * 
 * @return TASK_NODE* 
 */
TASK_NODE* get_current_task(void);

/**
 * @brief 当前任务延时
 * 
 * @param ms 
 */
void task_delay(uint32_t ms);

/**
 * @brief 获取当前任务的栈空间使用率
 * 
 * @return float  
 */
float stack_usage();

void __asm save_stack_frame(void);

void yeild();

void resume();

#endif
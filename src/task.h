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
	TASK_FUNCTION f;		//������ں���
	void* param;			//�������
	TASK_PRIORITY priority; //�������ȼ�
	uint8_t *stack_base; 	//����ջ����ַ
	const char *task_name;	//������
	uint32_t stack_size;	//����ջ��С
	uint32_t *stack_top;	//����ջ��
	uint32_t *cur_stack;	//��ǰջָ��
	uint8_t isFirstRun;		//�Ƿ��һ������
}TASK_TCB;

typedef enum{
	TASK_DISABLED = 0x0,	//��ֹ����
	TASK_READY = 0x1,		//׼���õ���
	TASK_EXEC,				//��������
	TASK_TERM,				//����
	TASK_DELAY,				//��ʱ
}TASK_STATUS;

typedef struct TASK_NODE{
	TASK_TCB task;			//������ƿ�
	TASK_STATUS status;		//����״̬
	uint32_t delay_tick;	//��ʱ��tick
	struct TASK_NODE *pre;	//��һ������ڵ� �����ȼ�
	struct TASK_NODE *next;	//��һ������ڵ� �����ȼ�
}TASK_NODE;

typedef struct{
	TASK_NODE tasks[MAX_TASK_COUNT];	//���е�����ڵ�����
	uint32_t size;						//��ǰ��������
	TASK_NODE *highest_node;			//������ȼ�����
	TASK_NODE *lowest_node;				//������ȼ�����
}TASK_LIST;

/**
 * @brief ����һ������
 * 
 * @param task_name 	������
 * @param func 			������ں���
 * @param param 		�������
 * @param priority 		���ȼ�
 * @param stack 		����ջ
 * @param stack_size 	ջ��С
 * @return TASK_TCB* 	������ƿ�
 */
TASK_TCB* create_task(const char *task_name, TASK_FUNCTION func, void* param, TASK_PRIORITY priority, uint8_t *stack, uint32_t stack_size);

/**
 * @brief os��ʼ����
 * 
 */
void os_start_schedule(void);

/**
 * @brief os���Ⱥ���
 * 
 * @param lr_ 
 */
void os_schedule(void);

/**
 * @brief os�Ƿ�ʼ����
 * 
 * @return uint8_t 
 */
uint8_t is_started(void);

/**
 * @brief ��ȡ��ǰ����
 * 
 * @return TASK_NODE* 
 */
TASK_NODE* get_current_task(void);

/**
 * @brief ��ǰ������ʱ
 * 
 * @param ms 
 */
void task_delay(uint32_t ms);

/**
 * @brief ��ȡ��ǰ�����ջ�ռ�ʹ����
 * 
 * @return float  
 */
float stack_usage();

void __asm save_stack_frame(void);

void yeild();

void resume();

#endif
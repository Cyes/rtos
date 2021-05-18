#ifndef __TASK_H__
#define __TASK_H__

#include "kernel.h"
#include "list.h"

struct task_desc_t{
	void *stack;
	void *func;
	int id;
	void *param;
	float flt;
	int priority;
	int data;
	uint32_t sleep_mark;
	char name[8];
	struct list_head list;
} ;

extern struct list_head g_task_list;
extern struct task_desc_t *currentTD;

void create_daemon_task(void);
void next_context(void);
void task_create(struct task_desc_t *td,void *stack,int stack_size, \
                 void *func,void *param,int pri,void *name);



#endif

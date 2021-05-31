#ifndef __TASK_H__
#define __TASK_H__

#include "kernel.h"
#include "list.h"




struct task_desc_t{
	void *stack;
	void *stack_base;
	int  stack_deep;
	int uid:16;
	int prio:8;
	int prio_save:8;
	void *func;
	char *name;
	void *param;
	uint32_t tick;
	uint32_t run;
	struct list_head mirror;
	struct list_head list;

} ;


#define PRIORITY_BIT(x) ((1 << (31 -x)))
#define PRIORITY_ID(x) (__builtin_clz(x))

extern struct task_desc_t *currentTD;
extern struct task_list_t g_ready_task;
extern struct task_list_t g_sleep_task;







void next_context(void);
void task_info(char *buffer);
void task_info_space(char *buffer);
void *find_luckly_task(struct task_list_t *list);
void prio_bit_update(struct task_list_t *curlist,int prio,int stat);
void task_create(struct task_desc_t *td,void *stack,int stack_size, \
                 void *func,void *param,int prio,void *name);





#endif

#ifndef __TASK_H__
#define __TASK_H__

#include "kernel.h"
#include "list.h"


struct task_desc_t{
	void *stack;
	int uid:16;
	int prio:16;
	void *func;
	void *param;
	char name[8];
	uint32_t tick;
	struct list_head list;

} ;

	extern struct task_list_t g_ready_task;
	extern struct task_list_t g_sleep_task;

#define PRIORITY_BIT(x) ((1 << (31 -x)))
#define PRIORITY_ID(x) (__builtin_clz(x))


void task_list_move(struct task_desc_t *td, struct task_list_t *curlist, struct task_list_t *newlist);
void *find_luckly_task(struct task_list_t *list);





//extern struct list_head g_task_list;
extern struct task_desc_t *currentTD;

void create_daemon(void);
void next_context(void);
void task_create(struct task_desc_t *td,void *stack,int stack_size, \
                 void *func,void *param,int prio,void *name);



#endif

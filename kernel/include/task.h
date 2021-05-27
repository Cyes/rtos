#ifndef __TASK_H__
#define __TASK_H__

#include "kernel.h"
#include "list.h"

#define DEFAULT_STACK_BYTE (1024)


struct task_desc_t{
	void *topstack;
	int uid:16;
	int prio:16;
	void *base;
	void *func;
	char *name;
	void *param;
	int stack_deep;
	uint32_t tick;
	uint32_t run;
	struct list_head mirror;
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

void next_context(void);
void create_daemon(void);
void task_info(char *buffer);
void task_create(struct task_desc_t *td,void *stack,int stack_size, \
                 void *func,void *param,int prio,void *name);



#endif

#ifndef __TASK_H__
#define __TASK_H__

#include "kernel.h"




struct task_desc_t{
    
	void *stack;//fixed location first bytes
    void *stack_top;
	void *stack_base;
    int stack_used;
    int stack_deep;//max used
	int uid:16;
	int prio:8;
	int prio_save:8;
	void *func;
	char *name;
	void *param;
	unsigned int tick;
	unsigned int run;
	struct list_head mirror;
	struct list_head list;

} ;


#define PRIORITY_BIT(x) ((1 << (31 -x)))
#define PRIORITY_ID(x) (__builtin_clz(x))

extern struct task_desc_t *currentTD;
extern struct task_list_t g_ready_task;







void next_context(void);
void *find_luckly_task(struct task_list_t *list);
void prio_bit_update(struct task_list_t *curlist,int prio,int stat);
struct task_desc_t *task_create(int stack_size,void *func,void *param,int prio,void *name);
int task_create_static(struct task_desc_t *td,void *stack,int stack_size, \
                 void *func,void *param,int prio,void *name);





#endif

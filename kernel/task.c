#include "task.h"
#include <stdio.h>


#define IDLE_STACK_BYTE (256)
#define IDLE_TASK_PRIO (MAX_PRIO_NUMBER -1)
#define HANDLER_MODE_LR (0xFFFFFFFD)
#define THUMB_MODE_XPSR (0x01000000)

struct task_list_t del_task;
LIST_HEAD(g_task_mirror);

static int g_task_id = 0;
struct task_desc_t *currentTD;
struct task_desc_t td_idle;

struct psp_stack_t{
	int r14_last;
	
	int r0;
	int r1;
	int r2;
	int r3;
	 
	int r12;
	int lr;
	int pc;
	int xpsr;
};

struct task_list_t g_ready_task;
struct task_list_t g_sleep_task;

void task_info(char *buffer)
{
	extern uint32_t tmr;
	struct list_head *pos;
	struct task_desc_t *ptd;
	if(buffer == NULL) return ;
	
	int length = sprintf(buffer,"\nname\tuid\tprio\tstack\tcpu\n");
	list_for_each(pos,&g_task_mirror){
		ptd = list_entry(pos,struct task_desc_t,mirror);
		length += sprintf(buffer + length,"%s\t%d\t%d\t%d\t%.1f%%\n", \
							ptd->name,ptd->uid,ptd->prio,ptd->stack_deep,(float)ptd->run/tmr *100);
		ptd->run = 0;
	}
	tmr = 0;
}


void *find_luckly_task(struct task_list_t *list)
{
	struct list_head *pos;
	struct task_desc_t *ptd;
	//if(!list->prio_bit)printf("error bit\n");
	int id = PRIORITY_ID(list->prio_bit);
	
#if 0	
	printf("list: ");
	list_for_each(pos,&list->list[id]){
		ptd = list_entry(pos,struct task_desc_t,list);
		printf("[%s] ",ptd->name);
	}	
	printf("\n");
#endif	
	
	pos = list->list[id].next;
	list_move_tail(pos,&list->list[id]);
	return  list_entry(pos,struct task_desc_t,list);
}


void task_list_add(struct task_desc_t *td, struct task_list_t *list)
{
    list_add(&td->list, &list->list[td->prio]);
    (list->prio_bit) |= PRIORITY_BIT(td->prio);
    list->active ++;
}

void task_list_move(struct task_desc_t *td, struct task_list_t *curlist, struct task_list_t *newlist)
{
	int prio = td->prio;
	
	list_move(&td->list,&newlist->list[prio]);
	if(list_empty(&curlist->list[prio])){
		(curlist->prio_bit) &= ~PRIORITY_BIT(prio);
	}
	(newlist->prio_bit) |= PRIORITY_BIT(prio);
	curlist->active --;
	newlist->active ++;
	
}

void os_task_exit(void)
{
	task_list_move(currentTD,&g_ready_task,&del_task);
	list_del(&currentTD->mirror);
	os_yield();
	for(;;);
}


void task_create(struct task_desc_t *td,void *stack,int stack_size,void *pfunc,void *param,int prio,void *name)
{
	struct psp_stack_t *psk;
	
	td->uid = g_task_id ++;
	td->prio = prio % MAX_PRIO_NUMBER;
	td->name = name;
	if(stack == NULL){
		stack = os_malloc(stack_size);
	}
	td->stack_base = stack + stack_size;
	//stack_size = stack_size >> 2;
	//td->stack = (int *)stack  +stack_size -17;
	//psk = (void*)((int*)stack +stack_size -9);
	td->stack = (char *)td->stack_base - 68;
	psk = (void *)((char *)td->stack_base - 36);
	
	
	psk->r0 = (int)param;
	psk->pc = (int)pfunc;
	psk->lr = (int)os_task_exit;
	psk->xpsr = THUMB_MODE_XPSR;
	psk->r14_last = HANDLER_MODE_LR;	
	
	INIT_LIST_HEAD(&td->list);
	task_list_add(td,&g_ready_task);
	list_add_tail(&td->mirror,&g_task_mirror);

}

void next_context(void)
{
	currentTD = find_luckly_task(&g_ready_task);	
	currentTD->stack_deep = (currentTD->stack_base - currentTD->stack );

}
static void daemon_task(void *param)
{
	for(;;){
		//os_yield();
	}
}

void create_daemon(void)
{
	task_create(&td_idle,0,IDLE_STACK_BYTE,daemon_task,(void *)0,IDLE_TASK_PRIO,"daemon");
}



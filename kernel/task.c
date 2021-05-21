#include "task.h"
#include <stdio.h>


#define IDLE_TASK_STACK (256)
#define HANDLER_MODE_LR (0xFFFFFFFD)
#define THUMB_MODE_XPSR (0x01000000)

struct task_list_t del_task;
static LIST_HEAD(g_task_list_sub);
__attribute((aligned (8)))static int stack[IDLE_TASK_STACK]={0};

static int g_task_id = 0;
struct task_desc_t*currentTD;
static struct task_desc_t td_idle;

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


void *find_luckly_task(struct task_list_t *list)
{
	void *cur = NULL;
	struct list_head *pos;
	if(list->task_prio_bit ==0)printf("error. prio_bit = null \n");
	int id = PRIORITY_ID(list->task_prio_bit);
	pos = list->list[id].next;
	cur = list_entry(pos,struct task_desc_t,list);
	
	return cur;
}


void task_list_add(struct task_desc_t *td, struct task_list_t *list)
{
    list_add(&td->list, &list->list[td->prio]);
    (list->task_prio_bit) |= PRIORITY_BIT(td->prio);
    list->active ++;
}

void task_list_move(struct task_desc_t *td, struct task_list_t *curlist, struct task_list_t *newlist)
{
	int prio = td->prio;
	
	list_move(&td->list,&newlist->list[prio]);
	if(list_empty(&curlist->list[prio])){
		(curlist->task_prio_bit) &= ~PRIORITY_BIT(prio);
	}
	(newlist->task_prio_bit) |= PRIORITY_BIT(prio);
	curlist->active --;
	newlist->active ++;
	
}

void os_task_exit(void)
{
	task_list_move(currentTD,&g_ready_task,&del_task);
	os_yield();
	for(;;);
}

void task_create(struct task_desc_t *td,void *stack,int stack_size,void *pfunc,void *param,int prio,void *name)
{
	struct psp_stack_t *psk;
	
	td->uid = g_task_id ++;
	td->prio = prio%MAX_PRIO_NUMBER;
	strncpy(td->name,name,sizeof(td->name));
	td->stack = (int *)stack  +stack_size -17;
	
	psk = (void*)((int*)stack +stack_size -9);
	psk->r0 = (int)param;
	psk->pc = (int)pfunc;
	psk->lr = (int)os_task_exit;
	psk->xpsr = THUMB_MODE_XPSR;
	psk->r14_last = HANDLER_MODE_LR;	
	
	INIT_LIST_HEAD(&td->list);
	task_list_add(td,&g_ready_task);
}

void next_context(void)
{

	os_enter_critical();
	currentTD = find_luckly_task(&g_ready_task);
	os_exit_critical();
}

static void daemon_task(void *param)
{
	for(;;){
		os_yield();
	}
}

void create_daemon(void)
{
	task_create(&td_idle,&stack,IDLE_TASK_STACK,daemon_task,(void*)0,MAX_PRIO_NUMBER -1,"daemon");
}



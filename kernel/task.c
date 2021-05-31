#include "task.h"
#include <stdio.h>


#define IDLE_STACK_BYTE (256)
#define IDLE_TASK_PRIO (MAX_PRIO_NUMBER -1)
#define HANDLER_MODE_LR (0xFFFFFFFD)
#define THUMB_MODE_XPSR (0x01000000)

LIST_HEAD(g_task_mirror);
LIST_HEAD(g_suspend_task);

static int g_task_id = 0;
struct task_desc_t __idle;
struct task_desc_t *currentTD;
struct task_list_t g_ready_task;
struct task_list_t g_sleep_task;

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
	length += sprintf(buffer + length, "[ mem: min=%dk cur=%dk total=%dk ]", \
						(get_water_level()* CONFIG_HEAP_BLOCK)>>10,(get_free_block() * CONFIG_HEAP_BLOCK)>>10,CONFIG_HEAP_SIZE>>10);
	////历史最小剩余量/当前剩余量/总量
	tmr = 0;
}
void task_info_space(char *buffer)
{
	extern uint32_t tmr;
	struct list_head *pos;
	struct task_desc_t *ptd;
	if(buffer == NULL) return ;
	
	int length = sprintf(buffer,"name   uid  prio stack  cpu   \n");
	list_for_each(pos,&g_task_mirror){
		ptd = list_entry(pos,struct task_desc_t,mirror);
		length += sprintf(buffer + length,"%-7s%-5X%-5d%-7d%.1f%%  \n", \
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

void prio_bit_update(struct task_list_t *curlist,int prio,int stat)
{
	if(stat){
		(curlist->prio_bit) |= PRIORITY_BIT(prio);
		curlist->active ++;
	}else{
		if(list_empty(&curlist->list[prio])){
			(curlist->prio_bit) &= ~PRIORITY_BIT(prio);
			curlist->active --;
		}	
	}
}




void os_task_exit(void)
{
	list_move(&currentTD->list,&g_suspend_task);
	prio_bit_update(&g_ready_task,currentTD->prio,0);
	list_del(&currentTD->mirror);
	os_yield();
	for(;;);
}

static int use_init_tasl_list = false;
void task_create(struct task_desc_t *td,void *stack,int stack_size,void *pfunc,void *param,int prio,void *name)
{
	struct psp_stack_t *psk;
	
	if(false == use_init_tasl_list){
		use_init_tasl_list = true;		
		for(int i=0;i<MAX_PRIO_NUMBER;i++){
			INIT_LIST_HEAD(&g_ready_task.list[i]);
			INIT_LIST_HEAD(&g_sleep_task.list[i]);
		}		
	}
	
	td->uid = g_task_id ++;
	td->prio = prio % MAX_PRIO_NUMBER;
	td->name = name;
	if(stack == NULL){
		stack = os_malloc(stack_size);
	}
	td->stack_base = stack + stack_size;
	td->stack = (char *)td->stack_base - 68;
	psk = (void *)((char *)td->stack_base - 36);
	
	
	psk->r0 = (int)param;
	psk->pc = (int)pfunc;
	psk->lr = (int)os_task_exit;
	psk->xpsr = THUMB_MODE_XPSR;
	psk->r14_last = HANDLER_MODE_LR;	
	
	INIT_LIST_HEAD(&td->list);
	list_add(&td->list,&g_ready_task.list[td->prio]);
	prio_bit_update(&g_ready_task,td->prio,1);
	list_add_tail(&td->mirror,&g_task_mirror);

}

void next_context(void)
{
	currentTD = find_luckly_task(&g_ready_task);	
	currentTD->stack_deep = (currentTD->stack_base - currentTD->stack );

}





static void __idle_task(void *param)
{
	for(;;){
		
	}
}


void system_idle_task_create(void)
{
	task_create(&__idle,0,IDLE_STACK_BYTE,__idle_task,(void *)0,IDLE_TASK_PRIO,"idle");
}




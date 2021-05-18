#include "task.h"



#define IDLE_TASK_STACK (256)
#define HANDLER_MODE_LR (0xFFFFFFFD)
#define THUMB_MODE_XPSR (0x01000000)


LIST_HEAD(g_task_list);
static LIST_HEAD(del_list);
static LIST_HEAD(g_task_list_sub);
__attribute((aligned (8)))static int stack[IDLE_TASK_STACK]={0};

static int g_task_id = 0;
static struct task_desc_t td_idle;
struct task_desc_t*currentTD;









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

void os_task_exit(void)
{
	list_move(&currentTD->list,&del_list);
	os_yield();
	for(;;);
}

void task_create(struct task_desc_t *td,void *stack,int stack_size,void *pfunc,void *param,int pri,void *name)
{
	struct psp_stack_t *psk;
	
	td->id = g_task_id ++;
	td->priority = pri;
	strncpy(td->name,name,sizeof(td->name));
	td->stack = (int *)stack  +stack_size -17;
	psk = (void*)((int*)stack +stack_size -9);
	psk->r0 = (int)param;
	psk->pc = (int)pfunc;
	psk->lr = (int)os_task_exit;
	psk->xpsr = THUMB_MODE_XPSR;
	psk->r14_last = HANDLER_MODE_LR;	
	
	list_add(&td->list,&g_task_list);
}



//0 = resv
//1 = max priority
//...
//255 = min

void next_context(void)
{
	//no error check
	
	struct list_head   *pos = NULL;
	struct task_desc_t *cur = NULL;
	struct task_desc_t *top = NULL;
	
	os_enter_critical();
	
	//move sub list to main list 
	if(list_empty(&g_task_list)){
		list_splice(&g_task_list_sub,&g_task_list);
		INIT_LIST_HEAD(&g_task_list_sub);
	}

	//search high prioroty task
	list_for_each(pos,&g_task_list){
		cur = list_entry(pos,struct task_desc_t,list);
		top = (NULL == top)?cur:top;
		if(cur->priority < top->priority){
			top = cur;
		}
	}
	
	//search high prioroty task at sub list
	list_for_each(pos,&g_task_list_sub){
		cur = list_entry(pos,struct task_desc_t,list);
		if(cur->priority < top->priority){
			top = cur;
		}
	}
	
	//not move
	if(top->list.next != &g_task_list_sub){		
		list_move(&top->list,&g_task_list_sub);
	}
	
	currentTD = top;
	os_exit_critical();
	
}


static void daemon_task(void *param)
{
	for(;;){
		os_yield();
	}
}

void create_daemon_task(void)
{
	task_create(&td_idle,&stack,IDLE_TASK_STACK,daemon_task,(void*)0,255,"daemon");
}



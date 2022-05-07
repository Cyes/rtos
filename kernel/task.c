#include "task.h"

#define STACK_CHECK_FLAG (0XAAAAAAAA)

#define IDLE_STACK_BYTE (256)
#define IDLE_TASK_PRIO (MAX_PRIO_NUMBER -1)
#define HANDLER_MODE_LR (0xFFFFFFFD)
#define THUMB_MODE_XPSR (0x01000000)

LIST_HEAD(g_task_mirror);
LIST_HEAD(g_suspend_task);
LIST_HEAD(g_sleep_task);

static int g_task_id = 0;

struct task_desc_t *currentTD;
struct task_list_t g_ready_task;

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



void *find_luckly_task(struct task_list_t *list)
{
    //irq disable alread
	struct list_head *pos;
	struct task_desc_t *ptd;
	int id = PRIORITY_ID(list->prio_bit);
	
#ifdef DBG_READY_TASK_LIST	
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
    //must running 'irq disable'
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
    os_enter_critical();
	list_move(&currentTD->list,&g_suspend_task);
	prio_bit_update(&g_ready_task,currentTD->prio,0);
	list_del(&currentTD->mirror);
    os_exit_critical();
	os_yield();
	for(;;);
}

static int use_init_tasl_list = false;


int task_create_static(struct task_desc_t *td,void *stack,int stack_size,void *pfunc,void *param,int prio,void *name)
{
	
	os_enter_critical();
	
	struct psp_stack_t *psk;
	if(NULL == td || NULL == stack){
        return -1;
    }
    
	if(false == use_init_tasl_list){
		use_init_tasl_list = true;		
		for(int i=0;i<MAX_PRIO_NUMBER;i++){
			INIT_LIST_HEAD(&g_ready_task.list[i]);
		}		
	}
    
#ifdef  CONFIG_STACK_CHECK
    memset(stack,STACK_CHECK_FLAG,stack_size);
#endif 
    
	td->uid = g_task_id ++;
	td->prio = prio % MAX_PRIO_NUMBER;
	td->name = name;
    td->stack_deep = 0;
    td->stack_base = stack;
	td->stack_top = stack + stack_size;
	td->stack = (char *)td->stack_top - 68;
	psk = (void *)((char *)td->stack_top - 36);
	
	
	psk->r0 = (int)param;
	psk->pc = (int)pfunc;
	psk->lr = (int)os_task_exit;
	psk->xpsr = THUMB_MODE_XPSR;
	psk->r14_last = HANDLER_MODE_LR;	
	
	INIT_LIST_HEAD(&td->list);
	list_add(&td->list,&g_ready_task.list[td->prio]);
	prio_bit_update(&g_ready_task,td->prio,1);
	list_add_tail(&td->mirror,&g_task_mirror);
	
	os_exit_critical();
    return 0;
}


struct task_desc_t *task_create(int stack_size,void *pfunc,void *param,int prio,void *name)
{
	
	os_enter_critical();
	
	struct task_desc_t *td;
	struct psp_stack_t *psk;
    
	if(false == use_init_tasl_list){
		use_init_tasl_list = true;		
		for(int i=0;i<MAX_PRIO_NUMBER;i++){
			INIT_LIST_HEAD(&g_ready_task.list[i]);
		}
	}

    void *stack = os_malloc(stack_size);
    td = os_malloc(sizeof(struct task_desc_t));
#ifdef  CONFIG_STACK_CHECK
    memset(stack,STACK_CHECK_FLAG,stack_size);
#endif    
	if(NULL == td || NULL == stack){
        return NULL;
    }
    
	td->uid = g_task_id ++;
	td->prio = prio % MAX_PRIO_NUMBER;
	td->name = name;
    td->stack_deep = 0;
    td->stack_base = stack;
	td->stack_top = stack + stack_size;//top
	td->stack = (char *)td->stack_top - 68;
	psk = (void *)((char *)td->stack_top - 36);
	
	
	psk->r0 = (int)param;
	psk->pc = (int)pfunc;
	psk->lr = (int)os_task_exit;
	psk->xpsr = THUMB_MODE_XPSR;
	psk->r14_last = HANDLER_MODE_LR;	
	
	INIT_LIST_HEAD(&td->list);
	list_add(&td->list,&g_ready_task.list[td->prio]);
	prio_bit_update(&g_ready_task,td->prio,1);
	list_add_tail(&td->mirror,&g_task_mirror);
	
	os_exit_critical();
    return td;
}


void simple_stack_check(struct task_desc_t *td)
{
     //irq disable alread
    volatile int stack_free = 0;
    int stack_size = td->stack_top - td->stack_base;
    
    for(int i=0;i<stack_size;i+=4){
        int val = *(int*)(((char *)td->stack_base) + i);
        if(val == STACK_CHECK_FLAG){
            stack_free ++ ;
        }
    }
    td->stack_used = stack_size - (stack_free *4);
}


void next_context(void)
{
    //irq disable alread
	currentTD = find_luckly_task(&g_ready_task);	

#ifdef  CONFIG_STACK_CHECK
    simple_stack_check(currentTD);
#else
	currentTD->stack_used = (currentTD->stack_top - currentTD->stack );    
#endif
    currentTD->stack_deep = ( currentTD->stack_used > currentTD->stack_deep ) ?  \
        currentTD->stack_used : currentTD->stack_deep;
}


void *query_task_free(void)
{
	if(list_empty(&g_suspend_task)){
		struct list_head *next = g_suspend_task.next;
		struct task_desc_t *ptd = container_of(next,struct task_desc_t,list);
		return ptd->stack_top;
	}
	return 0;
}


static void __idle_task(void *param)
{
	for(;;){
		__WFI();
	}
}


void system_idle_task_create(void)
{
	task_create(IDLE_STACK_BYTE,__idle_task,(void *)0,IDLE_TASK_PRIO,"idle");
}




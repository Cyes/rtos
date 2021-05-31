#include "timer.h"
#include "task.h"

#ifdef CONFIG_USE_TIMER
LIST_HEAD(g_timers_task);
static struct task_desc_t __timer;
#endif


void os_tick_callback(void)
{
	struct task_desc_t *cur = NULL;
	struct list_head   *pos = NULL;
	struct list_head   *obj = NULL;
	struct list_head   *temp = NULL;
	
	for(int i = 0;i<MAX_PRIO_NUMBER;i++){
		obj = &g_sleep_task.list[i];
		if(!list_empty(obj)){
			list_for_each_safe(pos,temp,obj){
				cur = list_entry(pos,struct task_desc_t,list);
				if(cur->tick){
					cur->tick -- ;
				}
				if(!cur->tick){				
					list_move(&cur->list,&g_ready_task.list[cur->prio]);
					prio_bit_update(&g_ready_task,cur->prio,1);
					prio_bit_update(&g_sleep_task,cur->prio,0);
				}
			}
		}
	}
}


void os_sleep(int tick)
{
	
	struct list_head *pos;
	if(0 == tick){
		return;
	}
	os_schedule(false);
	currentTD->tick = tick;
	
	list_move(&currentTD->list,&g_sleep_task.list[currentTD->prio]);
	prio_bit_update(&g_ready_task,currentTD->prio,0);
	prio_bit_update(&g_sleep_task,currentTD->prio,1);
	
	os_schedule(true);
	os_yield();
	
}



#ifdef CONFIG_USE_TIMER

struct timer_t{
	void *func;
	void *param;
	int timeout;
	int reload;
	int mode;
	struct list_head list;
};


void periodic_task(void *func, void *param, int timeout, int mode)
{
	struct timer_t *ptimer = os_malloc(sizeof(struct timer_t));
	ptimer->func = func;
	ptimer->mode = mode;
	ptimer->param = param;
	ptimer->reload = timeout;
	ptimer->timeout = timeout;
	
	list_add(&ptimer->list,&g_timers_task);
}

static void __timer_task(void *param)
{
	struct timer_t *ptm    = NULL;
	struct list_head *pos  = NULL;
	struct list_head *temp = NULL;
	for(;;){
		
		list_for_each_safe(pos,temp,&g_timers_task){
			ptm = list_entry(pos,struct timer_t,list);
			if(--ptm->timeout == 0){
				void (*exec)(void *param);
				exec = ptm->func;
				exec(ptm->param);
				if(ptm->mode == PERIODIC_LOOP){
					ptm->timeout = ptm->reload;
				}else{
					list_del(&ptm->list);
				}
			}
		}
		
		os_sleep(1);
	}
}


void system_timer_task_create(void)
{
	task_create(&__timer,0,TIMER_STACK_BYTE,__timer_task,(void *)0,TIMER_TASK_PRIO,"timer");
}
#else

void system_timer_task_create(void){}

#endif


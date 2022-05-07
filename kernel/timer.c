#include "timer.h"
#include "queue.h"

#ifdef CONFIG_USE_TIMER
LIST_HEAD(g_timers_task);
struct queue_t *__qtimer;
#endif

extern struct list_head g_sleep_task;


void os_tick_callback(void)
{
	os_enter_critical();
	
	struct task_desc_t *cur = NULL;
	struct list_head   *pos = NULL;
	struct list_head   *temp = NULL;

    list_for_each_safe(pos,temp,&g_sleep_task){
        cur = list_entry(pos,struct task_desc_t,list);
        if(cur->tick){
            cur->tick -- ;
        }
        if(!cur->tick){
            list_move(&cur->list,&g_ready_task.list[cur->prio]);
            prio_bit_update(&g_ready_task,cur->prio,1);
            //non-update
        }
    }
	os_exit_critical();
}


void os_sleep(int tick)
{

    if(is_isr()) return ;
	if(0 == tick){
        os_yield();
        return;
    }
    
	os_enter_critical();
	struct list_head *pos;
	currentTD->tick = TICK(tick);
	list_move(&currentTD->list,&g_sleep_task);
	prio_bit_update(&g_ready_task,currentTD->prio,0);
	//non-update
	
	os_yield();
	os_exit_critical();
}



#ifdef CONFIG_USE_TIMER



void os_timer_callback(void){

    static int low = 100;
	struct timer_t *ptm    = NULL;
	struct list_head *pos  = NULL;
	struct list_head *temp = NULL;

	list_for_each_safe(pos,temp,&g_timers_task){
		ptm = list_entry(pos,struct timer_t,list);
		if(--ptm->timeout < 0){
           //if(__qtimer->free < low)low = __qtimer->free;
           //printf("timer = %d, %d\n",__qtimer->free,low);
            
            if(!queue_isfull(__qtimer)){
                int len = queue_write(__qtimer,(void *)&ptm,4,0);
                if(len > 0)
                    os_enter_critical();
                    list_del(&ptm->list);     
                    os_exit_critical();                
            }
            
		}
	}
		
}

struct timer_t *timer_john(void *func, int param, int timeout)
{
    if(NULL == func){
        return NULL;
    }
	struct timer_t *ptimer = os_malloc(sizeof(struct timer_t));
    if(NULL == ptimer){
        return NULL;
    }
    os_enter_critical();
	ptimer->func = func;
	ptimer->param = param;
	ptimer->reload = timeout;
	ptimer->timeout = timeout * SYSTEM_TICK_FREQ / 1000;
	list_add(&ptimer->list,&g_timers_task);
	os_exit_critical();
    return ptimer;
}

static void __timer_task(void *param){

    struct timer_t *ptimer;
    for(;;){
        
        int ret = queue_read(__qtimer,(void *)&ptimer,0);
        if(NULL == ptimer){
            printf("ptimer is NULL\n");
            return ;
        }
        if(ret > 0){
            int (*exec)(int,struct timer_t *) = ptimer->func;
            if(exec(ptimer->param,ptimer) > 0){
                timer_john(ptimer->func,ptimer->param,ptimer->reload);
            }
            os_free(ptimer);
        }
    }
}


void system_timer_task_create(void)
{

     __qtimer = queue_create(8,4);
 	task_create(TIMER_STACK_BYTE,__timer_task,(void *)0,TIMER_TASK_PRIO,"timer");
}
#else

void system_timer_task_create(void){}

#endif


#include "timer.h"
#include "task.h"


void os_sleep_callback(void)
{
	static int need_yield = 0;
	struct task_desc_t *cur = NULL;
	struct list_head   *pos = NULL;
	struct list_head   *obj = NULL;
	struct list_head   *temp = NULL;
	
	os_enter_critical();
	for(int i = 0;i<MAX_PRIO_NUMBER;i++){
		obj = &g_sleep_task.list[i];
		if(!list_empty(obj)){
			list_for_each_safe(pos,temp,obj){
				cur = list_entry(pos,struct task_desc_t,list);

				if(cur->tick){
					cur->tick -- ;
				}
				if(!cur->tick){
					task_list_move(cur,&g_sleep_task,&g_ready_task);
					if(cur->prio < currentTD->prio)need_yield = 1;
				}

			}
		}
	}
	os_exit_critical();
	if(need_yield)os_yield();
}


void os_sleep(int tick)
{
	os_enter_critical();
	if(0 == tick){
		os_exit_critical();
		return;
	}
	currentTD->tick = tick;
	{
		task_list_move(currentTD,&g_ready_task,&g_sleep_task);
		os_exit_critical();
		os_yield();
	}
}




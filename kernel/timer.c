#include "timer.h"
#include "task.h"


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
					task_list_move(cur,&g_sleep_task,&g_ready_task);
				}
			}
		}
	}
}


void os_sleep(int tick)
{
	if(0 == tick){
		return;
	}
	os_schedule(false);
	currentTD->tick = tick;
	task_list_move(currentTD,&g_ready_task,&g_sleep_task);
	os_schedule(true);
	os_yield();
	
}




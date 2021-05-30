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




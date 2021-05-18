#include "timer.h"
#include "task.h"


static LIST_HEAD(sleep_list);
#define MAX_COUNT_TIME ((uint32_t)(2^32 - 1))



void os_sleep_callback(void)
{
	struct task_desc_t *cur = NULL;
	struct list_head   *pos = NULL;
	struct list_head   *temp = NULL;
	os_enter_critical();
	
	list_for_each_safe(pos,temp,&sleep_list){
		cur = list_entry(pos,struct task_desc_t,list);
		if(cur->sleep_mark == os_get_tick()){
			list_move(&cur->list,&g_task_list);
		}
	}
	os_exit_critical();

}

void os_sleep(int tick)
{
	os_enter_critical();
	if(0 == tick){
		os_exit_critical();
		return;
	}
	currentTD->sleep_mark = os_get_tick() + tick;
	{
		list_move(&currentTD->list,&sleep_list);
		os_exit_critical();
		os_yield();
	}
}


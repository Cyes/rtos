#include "mutex.h"


void mutex_create(struct mutex_t *mux, int prio)
{
	mux->prio = prio;
	mux->lock = false;
	for(int i=0;i<MAX_PRIO_NUMBER;i++){
		INIT_LIST_HEAD(&mux->put);
		INIT_LIST_HEAD(&mux->get);
	}
}

int mutex_get(struct mutex_t *mux)
{
	if(mux->owner == currentTD)return 0;

	for(;;){	
		os_enter_critical();
		
		if(mux->lock == false){
			mux->owner = currentTD;
			currentTD->prio_save = currentTD->prio;
			currentTD->prio = mux->prio;
			list_move(&currentTD->list,&g_ready_task.list[mux->prio]);
			prio_bit_update(&g_ready_task,currentTD->prio_save,0);
			prio_bit_update(&g_ready_task,currentTD->prio,1);

			mux->lock = true;
			os_exit_critical();
			return true;
		}else{
			list_move(&currentTD->list,&mux->get);
			prio_bit_update(&g_ready_task,currentTD->prio,0);			
			os_yield();
			os_exit_critical();
		}	
		
		
	}
}

int mutex_put(struct mutex_t *mux)
{
		os_enter_critical();
	
		struct task_desc_t *ptd = NULL;
		if(mux->owner == currentTD){
			
			list_move(&currentTD->list,&g_ready_task.list[currentTD->prio_save]);
			prio_bit_update(&g_ready_task,currentTD->prio_save,1);
			prio_bit_update(&g_ready_task,mux->prio,0);			
			currentTD->prio = currentTD->prio_save;
			mux->owner = NULL;
			mux->lock = false;

			if(!list_empty(&mux->get)){
				struct list_head *pos = mux->get.next;
				ptd = list_entry(pos,struct task_desc_t,list);
				list_move(pos,&g_ready_task.list[ptd->prio]);
				prio_bit_update(&g_ready_task,currentTD->prio,1);		
				if(ptd->prio < currentTD->prio){
					os_yield();
				}			
			}
			
		}
		
		os_exit_critical();
		return 0;	
}
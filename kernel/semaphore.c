#include "semaphore.h"


void semaphore_create(struct semaphore_t *sem, int initial_val, int max_val)
{
    os_enter_critical();
	sem->max_val = max_val;
	sem->counter = initial_val;
	for(int i=0;i<MAX_PRIO_NUMBER;i++){
		INIT_LIST_HEAD(&sem->put.list[i]);
		INIT_LIST_HEAD(&sem->get.list[i]);
	}
	os_exit_critical();
}


int semaphore_get(struct semaphore_t *sem)
{    
	for(;;){
		os_enter_critical();
		struct task_desc_t *ptd = NULL;
        
		if(sem->counter >0){
			if(sem->put.prio_bit){
				ptd = find_luckly_task(&sem->put);
				list_move(&ptd->list,&g_ready_task.list[ptd->prio]);
				prio_bit_update(&g_ready_task,ptd->prio,1);
				prio_bit_update(&sem->put,ptd->prio,0);
			
				if(ptd->prio < currentTD->prio){
					os_yield();
				}
			}
			sem->counter --;
			os_exit_critical();
			return true;
		}else{
            if(is_isr()){
                os_exit_critical();
                return false;
            } 
			list_move(&currentTD->list,&sem->get.list[currentTD->prio]);
			prio_bit_update(&g_ready_task,currentTD->prio,0);
			prio_bit_update(&sem->get,currentTD->prio,1);
			os_yield();
			os_exit_critical();
		}		
	}
	
}


int semaphore_put(struct semaphore_t *sem)
{
	
	for(;;){
		os_enter_critical();
		struct task_desc_t *ptd = NULL;
		if(is_isr() || sem->counter < sem->max_val){
			if(sem->get.prio_bit){
				ptd = find_luckly_task(&sem->get);
				list_move(&ptd->list,&g_ready_task.list[ptd->prio]);
				prio_bit_update(&g_ready_task,ptd->prio,1);
				prio_bit_update(&sem->get,ptd->prio,0);
			
				if(ptd->prio < currentTD->prio){
					os_yield();
				}
			}
			//sem->counter ++;
            sem->counter = (sem->counter < sem->max_val) ? sem->counter +1 : sem->counter;
			os_exit_critical();
			return true;
		}else{
			list_move(&currentTD->list,&sem->put.list[currentTD->prio]);
			prio_bit_update(&g_ready_task,currentTD->prio,0);
			prio_bit_update(&sem->put,currentTD->prio,1);
				
			os_yield();
			os_exit_critical();
		}
	
	
	}

}
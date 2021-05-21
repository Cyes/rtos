#include "kernel.h"
#include "queue.h"
#include "task.h"



int queue_reset(struct queue_t *q, int count, int size, void *memery)
{
	if(memery == NULL){
		//malloc
	}else{
		q->size = size;
		q->memery = memery;
		q->write = q->read = 0;
		q->txlock = q->rxlock = 0;
		q->count = q->message = count;
		
		for(int i=0;i<MAX_PRIO_NUMBER;i++){
			INIT_LIST_HEAD(&q->tx.list[i]);
			INIT_LIST_HEAD(&q->rx.list[i]);
		}
	}
	return 0;
}

static inline int queue_isfull(struct queue_t *q)
{
	return (!q->message);
}

static inline int queue_isempty(struct queue_t *q)
{
	return (q->message == q->count);
}

int queue_write(struct queue_t *q, const char *buffer, int length, int tick)
{
	(void)tick;

	struct task_desc_t *cur = NULL;
	struct list_head   *pos = NULL;
	struct list_head   *temp = NULL;
	
	for(;;){
		os_enter_critical();
		if(!queue_isfull(q)){
			length = (length > q->size)?q->size:length;
			char *writeto = (char *)q->memery + (q->write * q->size);
			memset((void *)writeto, 0, q->size);
			memcpy((void *)writeto, buffer, length);
			q->message --;
			q->write ++;
			q->write %= q->count;
	
			if(q->rx.task_prio_bit){
				cur = find_luckly_task(&q->rx);
				task_list_move(cur,&q->rx ,&g_ready_task);	
			}	
			os_exit_critical();
			
			if(cur->prio > currentTD->prio){
				os_yield();
			}
			
			return 0;
		}else{
			task_list_move(currentTD,&g_ready_task,&q->tx);
			os_exit_critical();
			os_yield();
		}
	}
}

int queue_read(struct queue_t *q, char *buffer, int length, int tick)
{
	(void)tick;

	struct task_desc_t *cur = NULL;
	struct list_head   *pos = NULL;
	struct list_head   *temp = NULL;
	
	for(;;){	
		os_enter_critical();
		if(!queue_isempty(q)){
			length = (length > q->size)?q->size:length;
			char *readfrom = (char *)q->memery + (q->read * q->size);
			memcpy((void *)buffer, readfrom, length);
			q->message ++;
			q->read ++;
			q->read %= q->count;	
		
			if(q->tx.task_prio_bit){
				cur = find_luckly_task(&q->tx);
				task_list_move(cur,&q->tx ,&g_ready_task);
			}	
			os_exit_critical();
			
			if(cur->prio > currentTD->prio){
				os_yield();
			}	

			return 0;
		}else{
			task_list_move(currentTD,&g_ready_task,&q->rx);
			os_exit_critical();
			os_yield();
		}
	}	
}


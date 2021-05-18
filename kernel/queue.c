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
		INIT_LIST_HEAD(&q->task_tx);
		INIT_LIST_HEAD(&q->task_rx);
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
#if 0		
			list_for_each_safe(pos,temp,&q->task_rx){
				cur = list_entry(pos,struct task_desc_t,list);
				if(cur->priority < priority){
					obj = cur;
					priority = cur->priority;
				}
			}
			list_move(&obj->list,&g_task_list);		
#else
			list_for_each_safe(pos,temp,&q->task_rx){
				cur = list_entry(pos,struct task_desc_t,list);
				list_move(&cur->list,&g_task_list);
			}
			os_exit_critical();
			return 0;
#endif		
		}else{
			
			list_move(&currentTD->list,&q->task_tx);
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
#if 0		
			list_for_each_safe(pos,temp,&q->task_tx){
				cur = list_entry(pos,struct task_desc_t,list);
				if(cur->priority < priority){
					obj = cur;
					priority = cur->priority;
				}
			}
			list_move(&obj->list,&g_task_list);			
#else
			list_for_each_safe(pos,temp,&q->task_tx){
				cur = list_entry(pos,struct task_desc_t,list);
				list_move(&cur->list,&g_task_list);
			}
			os_exit_critical();
			return 0;
#endif		
		}else{
			list_move(&currentTD->list,&q->task_rx);
			os_exit_critical();
			os_yield();
		}
	}	
}


#include "kernel.h"
#include "queue.h"
#include "task.h"


/*
11  28900-29090    48M       
10  20500-19800    32M
9   15000-15000    24M
8   9250 - 9220    16M
7   5200 - 5200    8M
6   1270 - 1296    4M 
5   186  -  336    2M
4


*/

int queue_create(struct queue_t *q, int count, int size, void *memery)
{
	if(memery == NULL){
		memery = os_malloc(count * size);
	}
	q->size = size;
	q->memery = memery;
	q->write = q->read = 0;
	q->txlock = q->rxlock = 0;
	q->count = q->message = count;
	
	for(int i=0;i<MAX_PRIO_NUMBER;i++){
		INIT_LIST_HEAD(&q->tx.list[i]);
		INIT_LIST_HEAD(&q->rx.list[i]);
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
	struct task_desc_t *ptd = NULL;
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
			if(q->rx.prio_bit){
				ptd = find_luckly_task(&q->rx);
				list_move(&ptd->list,&g_ready_task.list[ptd->prio]);
				prio_bit_update(&g_ready_task,ptd->prio,1);
				prio_bit_update(&q->rx,ptd->prio,0);
			
				if(ptd->prio < currentTD->prio){
					os_yield();
				}
			}
			os_exit_critical();
			return 0;
		}else{
			list_move(&currentTD->list,&q->tx.list[currentTD->prio]);
			prio_bit_update(&g_ready_task,currentTD->prio,0);
			prio_bit_update(&q->tx,currentTD->prio,1);
				
			os_exit_critical();
			os_yield();
			
		}
	}
}

int queue_read(struct queue_t *q, char *buffer, int length, int tick)
{
	(void)tick;
	struct task_desc_t *ptd = NULL;
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
			if(q->tx.prio_bit){
				ptd = find_luckly_task(&q->tx);
				list_move(&ptd->list,&g_ready_task.list[ptd->prio]);
				prio_bit_update(&g_ready_task,ptd->prio,1);
				prio_bit_update(&q->tx,ptd->prio,0);			
				
				if(ptd->prio < currentTD->prio){
					os_yield();
				}	
			}	
			os_exit_critical();
			return 0;
		}else{
			list_move(&currentTD->list,&q->rx.list[currentTD->prio]);
			prio_bit_update(&g_ready_task,currentTD->prio,0);
			prio_bit_update(&q->rx,currentTD->prio,1);
	
			os_exit_critical();
			os_yield();
		}
	}	
}





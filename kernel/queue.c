#include "kernel.h"
#include "queue.h"
#include "task.h"




#ifdef CONFIG_USE_OLD_QUEUE

int queue_create_old(struct queue_t *q, int count, int size, void *memery)
{
	os_enter_critical();
	
	if(memery == NULL){
		memery = os_malloc(count * size);
	}
	q->size = size;
	q->memery = memery;
	q->write = q->read = 0;
	q->txlock = q->rxlock = 0;
	q->count = q->free = count;
	
	for(int i=0;i<MAX_PRIO_NUMBER;i++){
		INIT_LIST_HEAD(&q->tx.list[i]);
		INIT_LIST_HEAD(&q->rx.list[i]);
	}
	
	os_exit_critical();
	return 0;
}

int queue_write(struct queue_t *q, const char *buffer, int length, int tick)
{
	(void)tick;
	for(;;){
		os_enter_critical();
		struct task_desc_t *ptd = NULL;
		struct list_head   *pos = NULL;
		struct list_head   *temp = NULL;
		
		if(!queue_isfull(q)){
			length = (length > q->size)?q->size:length;
			char *writeto = (char *)q->memery + (q->write * q->size);
			memset((void *)writeto, 0, q->size);
			memcpy((void *)writeto, buffer, length);
			q->free --;
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
				
			os_yield();
			os_exit_critical();
			
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
			q->free ++;
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
	
			os_yield();
			os_exit_critical();
		}
	}	
}


#endif


#define ALIGN_BYTES (8)
/*queue_create 预留字节预存写入的字节数*/
struct queue_t *queue_create(int count, int size)
{
	os_enter_critical();
	
    struct queue_t *q;
    int bufsize = align(sizeof(struct queue_t),ALIGN_BYTES) + align(count * size,ALIGN_BYTES) + align(count * 4,ALIGN_BYTES);
    void *memery = os_malloc(bufsize);
    
    q = memery;
    q->psize = memery + align(sizeof(struct queue_t),ALIGN_BYTES);
    q->memery = memery + align(sizeof(struct queue_t),ALIGN_BYTES) + align(count * 4,ALIGN_BYTES);
	q->size = size;
	q->write = q->read = 0;
	q->txlock = q->rxlock = 0;
	q->count = q->free = count;
	
	for(int i=0;i<MAX_PRIO_NUMBER;i++){
		INIT_LIST_HEAD(&q->tx.list[i]);
		INIT_LIST_HEAD(&q->rx.list[i]);
	}
	
	os_exit_critical();
	return q;
}


int queue_write(struct queue_t *q, const char *buffer, int length, int tick)
{
	(void)tick;
    if(NULL == q) return -1;
	for(;;){
		os_enter_critical();
		struct task_desc_t *ptd = NULL;
		struct list_head   *pos = NULL;
		struct list_head   *temp = NULL;
		
		if(!queue_isfull(q)){
			length = (length > q->size)?q->size:length;
			char *writeto = (char *)q->memery + (q->write * q->size);
			memset((void *)writeto, 0, q->size);
            if(buffer == NULL || NULL == writeto){
                os_exit_critical();
                return -2;
            }

			memcpy((void *)writeto, buffer, length);
            *((uint32_t*)q->psize  + q->write) = length;
			q->free --;
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
			return length;
		}else{
            if(is_isr()){
                os_exit_critical();
                return 0;
            } 
			list_move(&currentTD->list,&q->tx.list[currentTD->prio]);
			prio_bit_update(&g_ready_task,currentTD->prio,0);
			prio_bit_update(&q->tx,currentTD->prio,1);
				
			os_yield();
			os_exit_critical();			
		}
        
	}
}


int queue_read(struct queue_t *q, char *buffer, int tick)
{
	(void)tick;
    if(NULL == q) return -1;
    uint32_t nbyte = 0;
	struct task_desc_t *ptd = NULL;
	struct list_head   *pos = NULL;
	struct list_head   *temp = NULL;
	
	for(;;){	
		os_enter_critical();
		if(!queue_isempty(q)){
            nbyte = *((uint32_t*)q->psize  + q->read);
			char *readfrom = (char *)q->memery + (q->read * q->size);
            if(buffer == NULL || NULL == readfrom){
                os_exit_critical();
                return -2;
            }
			memcpy((void *)buffer, readfrom, nbyte);
			q->free ++;
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
			return nbyte;
		}else{
            if(is_isr()){
                os_exit_critical();
                return 0;
            }
            //TODO:  add non-block parse
			list_move(&currentTD->list,&q->rx.list[currentTD->prio]);
			prio_bit_update(&g_ready_task,currentTD->prio,0);
			prio_bit_update(&q->rx,currentTD->prio,1);
	
			os_yield();
			os_exit_critical();
		}
	}	
}





inline int queue_isfull(struct queue_t *q)
{
	return (!q->free);
}

inline int queue_isempty(struct queue_t *q)
{
	return (q->free == q->count);
}



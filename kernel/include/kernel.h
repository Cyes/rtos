#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "stm32l4xx_hal.h"
#include <string.h>
#include <stdint.h>
#include "list.h"


#define MAX_PRIO_NUMBER (32)  //support 32 level priority max


struct task_list_t{
		unsigned int init:1;                       //
		unsigned int active:31;                    //total task
    unsigned int task_prio_bit;                //[(MAX_PRI_NUMBER +31)/32];     //bit flag
    struct list_head list[MAX_PRIO_NUMBER];    //就绪任务链表数组
};

void os_yield(void);
void os_start(void);
void os_inc_tick(void);
void os_exit_critical(void);
void os_enter_critical(void);
void os_heartbeat(int status);


#endif

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "stm32l4xx_hal.h"
#include "dynamic_block.h"
#include <string.h>
#include <stdint.h>
#include "list.h"
#include "task.h"
#include "profile.h"
#include "st7789.h"


#define TICK(x) (x * SYSTEM_TICK_FREQ / 1000U)
#ifndef false
#define false (0)
#define true  (!false)
#endif
#define printk printf

struct task_list_t{
		unsigned int active;
    unsigned int prio_bit;
    struct list_head list[MAX_PRIO_NUMBER];
};


void os_yield(void);
void os_start(void);
void os_inc_tick(void);
void os_exit_critical(void);
void os_enter_critical(void);
void os_heartbeat(int status);
void os_schedule(int status);


#endif

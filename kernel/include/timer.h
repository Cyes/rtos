#ifndef __TIMER_H__
#define __TIMER_H__

#include "kernel.h"


void os_sleep(int tick);
void os_tick_callback(void);


#ifdef CONFIG_USE_TIMER
enum{
	PERIODIC_ONCE = 0,
	PERIODIC_LOOP,
};
void periodic_task(void *func, void *param, int timeout, int mode);
#endif

#endif

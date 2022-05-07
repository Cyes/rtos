#ifndef __TIMER_H__
#define __TIMER_H__

#include "kernel.h"


void os_sleep(int tick);
void os_tick_callback(void);


//#ifdef CONFIG_USE_TIMER
struct timer_t{
	void *func;
	int param;
	int timeout;
	int reload;
	struct list_head list;
};

struct timer_t *timer_john(void *func, int param, int timeout);
void os_timer_callback(void);

//#endif

#endif

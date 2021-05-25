#ifndef __TIMER_H__
#define __TIMER_H__

#include "kernel.h"

void os_sleep(int tick);
void os_tick_callback(void);


#endif

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "stm32l4xx_hal.h"
#include <string.h>
#include <stdint.h>
#include "list.h"


void os_yield(void);
void os_start(void);
void os_inc_tick(void);
uint32_t os_get_tick(void);
void os_exit_critical(void);
void os_enter_critical(void);
void os_heartbeat(int status);


#endif

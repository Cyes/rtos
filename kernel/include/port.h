#ifndef __PORT_H__
#define __PORT_H__

#include "stm32l4xx_hal.h"
#include "kernel.h"

void os_svc(void);
void set_pendsv(void);
void os_enable_irq(void);
void os_disable_irq(void);
void set_systick(int status);

int is_isr(void);

#endif


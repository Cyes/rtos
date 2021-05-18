#include "kernel.h"
#include "timer.h"
#include "task.h"
#include "port.h"



static LIST_HEAD(dev_list);
static int cnt_critical = 0;
static uint32_t g_os_tick = 1;


uint32_t os_get_tick(void)
{
	return g_os_tick;
}

void os_inc_tick(void)
{
	g_os_tick ++;
	os_sleep_callback();
}

void os_enter_critical(void)
{
	__disable_irq();
	cnt_critical ++;
}

void os_exit_critical(void)
{
	if(-- cnt_critical == 0){
		__enable_irq();
	}
}

void os_yield(void)
{
	SCB->ICSR |= 1<< 28; //set pendsv
}


void os_start(void)
{
	create_daemon_task();//IDLE
	next_context();
	os_heartbeat(ENABLE);
	os_svc();
	for(;;);
}

void os_heartbeat(int status)
{
	if(status){
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//1UL
	}else{
		SysTick->CTRL &= ~ (SysTick_CTRL_ENABLE_Msk);
	}
}



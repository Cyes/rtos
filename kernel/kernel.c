#include "kernel.h"
#include "timer.h"
#include "task.h"
#include "port.h"




static LIST_HEAD(dev_list);
static int g_critical_cnt = 0;
static int g_schedule_cnt = 0;

void os_schedule(int status)
{
	os_enter_critical();
	g_schedule_cnt = (status) ? (g_schedule_cnt-1) : (g_schedule_cnt+1) ;
	os_exit_critical();
}

void os_inc_tick(void)
{
	os_enter_critical();
	os_tick_callback();
	if(!g_schedule_cnt){
		os_yield();
	}
	os_exit_critical();
}

void os_enter_critical(void)
{
	os_disable_irq();
	g_critical_cnt ++;
}

void os_exit_critical(void)
{
	if(-- g_critical_cnt == 0){
		os_enable_irq();
	}
}

void os_yield(void)
{
	SCB->ICSR |= 1<< 28; //set pendsv
}





void os_start(void)
{
	create_daemon();//IDLE
	next_context();
	*(uint32_t*)0xE000ED20 = 0xF0F00000;
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



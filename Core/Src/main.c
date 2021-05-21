#include "stm32l4xx_hal.h"
#include "kernel.h"
#include "driver.h"
#include "queue.h"
#include "timer.h"
#include "task.h"
#include "bsp.h"


//STM32L432KC,MAX80MHZ,256KFLASH,64KSRAM

#define STACK_SIZE_INT (256)
static __attribute((aligned (8))) int stack[4][STACK_SIZE_INT]={0};
static __attribute((aligned (8))) char q_buffer[128];

static struct task_desc_t td1,td2,td3,td4;
static struct fops_t uart2_fops;
static struct queue_t test_q;
static struct dev_t uart2;



static int func1(void *param)
{
	(void)param;
	for(;;){
		os_sleep(300);
		HAL_GPIO_TogglePin(LD3_GPIO_Port,LD3_Pin);
	}
}

static int func2(void *param)
{
	(void)param;
	static int data;
	for(;;){
		data ++;
		queue_write(&test_q,"task2",5,0);
		os_sleep(1000);
#if 1		
		if(data > 20){
			queue_write(&test_q,"** task2  bye **",16,0);
			return 0;
		}
#endif		
	}
}

static int func3(void *param)
{	
	(void)param;
	char buffer[32];
	for(;;){
		os_sleep(100);
		queue_read(&test_q,buffer,16,0);
		printf("task3[ %s ]\n",buffer);	
	}
}

static int func4(void *param)
{
	(void)param;
	char buffer[32];
	static float flt;
	float a = 1.14f,b = 1.1231f,c = 1.117f,d = 1.113f,e = 1.114f,f=0.01f;
	for(;;){
		flt += (a*b*c*d*e*f)*1.1416f;
		snprintf(buffer,32,"task4 = %f\n",(double)flt);
		queue_write(&test_q,buffer,(int)strlen(buffer),0);
		os_sleep(2000);
	}
}



int main(void)
{
	
  HAL_Init();
	MX_GPIO_Init();
	SystemClock_Config();//32mhz
	os_heartbeat(DISABLE);//1000hz
	MX_USART2_UART_Init();//115200-N-8-1
	MX_TIM7_Init();//10000hz irq

	uart2_fops.write = uart_send;
	uart2.io = uart2_fops;
	uart2.io.write(" run\n",5);
	
	queue_reset(&test_q,4,32,q_buffer);

#if 1	
	for(int i=0;i<MAX_PRIO_NUMBER;i++){
		INIT_LIST_HEAD(&g_ready_task.list[i]);
		INIT_LIST_HEAD(&g_sleep_task.list[i]);
	}
#endif
	
	/*          &TD   STACK    STACK_SIZE             FUNC   PARAM  PRIO  NAME */
	task_create(&td1,&stack[0],STACK_SIZE_INT,(void *)func1,(void*)0, 1 ,"task1");
	task_create(&td2,&stack[1],STACK_SIZE_INT,(void *)func2,(void*)0, 2 ,"task2");
	task_create(&td3,&stack[2],STACK_SIZE_INT,(void *)func3,(void*)0, 2 ,"task3");
	task_create(&td4,&stack[3],STACK_SIZE_INT,(void *)func4,(void*)0, 3 ,"task4");

	
	os_start();

}


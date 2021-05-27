#include "stm32l4xx_hal.h"
#include "kernel.h"
#include "driver.h"
#include "queue.h"
#include "timer.h"
#include "bsp.h"



//STM32L432KC,MAX80MHZ,256KFLASH,64KSRAM


static struct task_desc_t td[8];
static struct fops_t uart2_fops;
static struct queue_t q1,q2;
static struct dev_t uart2;



static int func5(void *param)
{
	(void)param;
	char pbuf[8];
	//char *pbuf = os_malloc(128);
	for(;;){
		//os_sleep(TICK(10));
		//s_free(os_malloc(300));
		queue_read(&q2,pbuf,8,0);
		if(!memcmp(pbuf,"AAAAAAAAA",8)){
			
		}else{
			printf("%s\n",pbuf);
		}
	}
}

static int func6(void *param)
{
	(void)param;
	//char pbuf[8];
	//char *pbuf = os_malloc(128);
	for(;;){
		//os_free(os_malloc(10));
		queue_write(&q2,"AAAAAAAA",8,0);
		//os_sleep(TICK(10));
	}
}

/*
extern uint32_t tmr;
extern struct task_desc_t td_idle;
extern struct list_head g_task_mirror;
struct list_head *pos;
struct task_desc_t *ptd;
*/
static int func1(void *param)
{
	(void)param;
	for(;;){
		os_sleep(TICK(500));
		HAL_GPIO_TogglePin(LD3_GPIO_Port,LD3_Pin);
		char *pbuf = os_malloc(512);
		task_info(pbuf);
		printf("%s\n",pbuf);
		os_free(pbuf);
		
	}
}

static int func2(void *param)
{
	(void)param;
	static int data;
	for(;;){
		//data ++;
		queue_write(&q1,"BBBBBBBB",8,0);
		//os_free(os_malloc(1024));
		os_sleep(TICK(10));
#if 0		
		if(data > 10){
			queue_write(&q1,"** task2  bye **",16,0);
			return 0;
		}
#endif		
	}
}

static int func3(void *param)
{	
	(void)param;
	char *pbuf = os_malloc(32);
	for(;;){
		//os_sleep(TICK(50));
		//os_sleep(TICK(100));
		queue_read(&q1,pbuf,8,0);
		os_free(os_malloc(10));
		if(!memcmp(pbuf,"BBBBBBBB",8)){

		}else{
			printf("%s\n",pbuf);
		}

		
	}
}
#include "md5.h"

static int func4(void *param)
{
	(void)param;
	for(;;){
		static ST_MD5 *md5;
		md5 = os_malloc(sizeof(ST_MD5));
		MD5_Handler(md5,"123;sdjgoasng;kasbjfasgasgf",16);
		os_free(md5);
		os_sleep(TICK(1));
		//os_free(os_malloc(600));
	}
}


int main(void)
{
  HAL_Init();
	MX_GPIO_Init();
	SystemClock_Config();    //32MHZ
	os_heartbeat(DISABLE);     //1000HZ
	MX_USART2_UART_Init();     //115200-N-8-1
	MX_TIM7_Init();            //10000HZ IRQ

	
	uart2_fops.write = uart_send;
	uart2.io = uart2_fops;
	uart2.io.write(" run\n",5);
	
	
	queue_reset(&q1,8,32,os_malloc(256));
	queue_reset(&q2,8,32,os_malloc(256));
	os_malloc(18);

#if 1
	for(int i=0;i<MAX_PRIO_NUMBER;i++){
		INIT_LIST_HEAD(&g_ready_task.list[i]);
		INIT_LIST_HEAD(&g_sleep_task.list[i]);
	}
#endif
	
	
	/*          &TD   STACK  STACK_SIZE             FUNC   PARAM  PRIO  NAME */
	task_create(&td[0],0,DEFAULT_STACK_BYTE,(void *)func1,(void*)0, 1 ,"task1");
	
	task_create(&td[1],0,DEFAULT_STACK_BYTE,(void *)func2,(void*)0, 6 ,"task2");
	task_create(&td[2],0,DEFAULT_STACK_BYTE,(void *)func3,(void*)0, 6 ,"task3");//read
	task_create(&td[3],0,DEFAULT_STACK_BYTE,(void *)func4,(void*)0, 1 ,"task4");
	task_create(&td[4],0,DEFAULT_STACK_BYTE,(void *)func5,(void*)0, 8 ,"task5");//read
	task_create(&td[5],0,DEFAULT_STACK_BYTE,(void *)func6,(void*)0, 8 ,"task6");

	os_start();

}


#include "stm32l4xx_hal.h"
#include "kernel.h"
#include "driver.h"
#include "queue.h"
#include "timer.h"
#include "bsp.h"
#include "st7789.h"
#include "semaphore.h"
#include "mutex.h"





//STM32L432KC,MAX80MHZ,256KFLASH,64KSRAM


static struct task_desc_t td[8];
static struct fops_t uart2_fops;
static struct queue_t q1,q2;
static struct dev_t uart2;
struct mutex_t mutex;
struct semaphore_t sem;




static int func1(void *param)
{
	(void)param;
	for(;;){
		os_sleep(TICK(100));
		HAL_GPIO_TogglePin(LD3_GPIO_Port,LD3_Pin);
		
	}
}

static int func2(void *param)
{
	(void)param;
	static int data;	
	for(;;){

		queue_write(&q1,"BBBBBBBB",8,0);
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
	char *p = 0;
	static uint64_t tick;
	char str[500] = {0};
	for(;;){
		static ST_MD5 *md5;
		
		p = os_malloc(1);
		os_sleep(TICK(5000));
		os_free(p);
		
		md5 = os_malloc(sizeof(ST_MD5));
		MD5_Handler(md5,(uint8_t *)"123",3);
		os_free(md5);		
		os_sleep(1000 *3600);
		td[1].uid ++; //test running time 

	
	}
}


static int func5(void *param)
{
	(void)param;
	char *pbuf;
	for(;;){
			
		mutex_get(&mutex);
		mem_info();
		os_sleep(TICK(2000));
		mutex_put(&mutex);
		os_sleep(TICK(2000));
	}
	
}
static int func6(void *param)
{
	(void)param;
	for(;;){
		char *pbuf = os_malloc(512);
		mutex_get(&mutex);
		task_info_space(pbuf);
		lcd_drawstring(0,0,pbuf,RED,WHITE);
		os_sleep(2000);
		mutex_put(&mutex);
		os_sleep(2000);		
		os_free(pbuf);
		
	}
}

void t1(void)
{
	printf("t1\n");
}

void t2(void *param)
{
	printf("%s\n",(char *)param);
}


int main(void)
{

  HAL_Init();
	MX_GPIO_Init();
	SystemClock_Config();    //32MHZ
	os_heartbeat(DISABLE);     //1000HZ
	MX_USART2_UART_Init();     //115200-N-8-1
	MX_TIM7_Init();            //10000HZ IRQ
	MX_SPI1_Init();
	lcd_init();

	
	uart2_fops.write = uart_send;
	uart2.io = uart2_fops;
	uart2.io.write(" run\n",5);
	
	semaphore_create(&sem,0,3);
	mutex_create(&mutex,2);

	queue_create(&q1,8,32,0);
	queue_create(&q2,8,32,0);


	//periodic_task(t1,0,1000,1);
	//periodic_task(t2,"timer2",2000,0);
	
	
	
	/*          &TD   STACK  STACK_SIZE             FUNC   PARAM  PRIO  NAME */
	task_create(&td[1],0,DEFAULT_STACK_BYTE,(void *)func1,(void*)0, 3 ,"task1");
	task_create(&td[2],0,DEFAULT_STACK_BYTE,(void *)func2,(void*)0, 3 ,"task2");
	task_create(&td[3],0,DEFAULT_STACK_BYTE,(void *)func3,(void*)0, 3 ,"task3");//read
	task_create(&td[4],0,DEFAULT_STACK_BYTE,(void *)func4,(void*)0, 4 ,"task4");
	task_create(&td[5],0,DEFAULT_STACK_BYTE,(void *)func5,(void*)0, 4 ,"task5");//read
	task_create(&td[6],0,DEFAULT_STACK_BYTE,(void *)func6,(void*)0, 4 ,"task6");

	os_start();

}


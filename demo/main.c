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

static struct queue_t *q1;
static struct dev_t uart2;
struct mutex_t mutex;



void task_info(char *buffer)//serial output format
{
	extern uint32_t tmr;
	struct list_head *pos;
	struct task_desc_t *ptd;
    extern struct list_head g_task_mirror;
	if(buffer == NULL) return ;
	
	int length = sprintf(buffer,"\nname\tuid\tprio\tstack\tcpu\n");
	list_for_each(pos,&g_task_mirror){
		ptd = list_entry(pos,struct task_desc_t,mirror);
		length += sprintf(buffer + length,"%s\t%d\t%d\t%d\t%.1f%%\n", \
							ptd->name,ptd->uid,ptd->prio,ptd->stack_deep,(float)ptd->run/tmr *100);
		ptd->run = 0;
	}
	length += sprintf(buffer + length, "[ memery: min=%dk cur=%dk total=%dk ]", \
						(get_water_level()* CONFIG_HEAP_BLOCK)>>10,(get_free_block() * CONFIG_HEAP_BLOCK)>>10,CONFIG_HEAP_SIZE>>10);
	////历史最小剩余量/当前剩余量/总量
	tmr = 0;
}


void task_info_space(char *buffer)//lcd display format
{
	extern uint32_t tmr;
	struct list_head *pos;
	struct task_desc_t *ptd;
    extern struct list_head g_task_mirror;
	if(buffer == NULL) return ;
	
	int length = sprintf(buffer,"name   uid  prio stack  cpu   \n");
	list_for_each(pos,&g_task_mirror){
		ptd = list_entry(pos,struct task_desc_t,mirror);
		length += sprintf(buffer + length,"%-7s%-5X%-5d%-7d%.1f%%  \n", \
							ptd->name,ptd->uid,ptd->prio,ptd->stack_deep,(float)ptd->run/tmr *100);
		ptd->run = 0;
	}
	tmr = 0;
}


void mem_info_little(void)
{
	const struct mem_t *pm = get_mem_desc();
	int length = 0;
	int c_index;
	int color = RED;
	int x = 0,y = 0;
	for(int i=0;i<CONFIG_MEM_BLOCK;i++){
		if(pm->flag[i]){
			
			color = (c_index)?RED:BLUE;		
			lcd_drawpix_buffer(x,y,color);
			if(!GET_BIT_NEXT(pm->flag[i])){
				c_index = !c_index;
			}			
		}else{
			lcd_drawpix_buffer(x,y,GREEN);
		}
		x += 4;
		if(x >240){
			x = 0;
			y+=4;
		}
	}
    lcd_fill(0,y +4,240,135,WHITE);

}


static int func1(void *param)
{
	(void)param;
	for(;;){
		os_sleep((100));
		HAL_GPIO_TogglePin(LD3_GPIO_Port,LD3_Pin);
		
	}
}

static int func2(void *param)
{
	(void)param;
	static int data;	
	for(;;){

		queue_write(q1,"hello",5,0);
		os_sleep((1000));
#if 1		
		if(++data >= 3){
			queue_write(q1,"** task2  bye **",16,0);
			return 0;
		}
#endif		
	}
}

static int func3(void *param)
{	
	(void)param;
	for(;;){
        char *pbuf = os_malloc(32);
		int recv_length = queue_read(q1,pbuf,0);
        printf("queue: [%d][%s]\n",recv_length,pbuf);
        if(strstr(pbuf,"bye")){
            lcd_fill(0,0,LCD_WIDTH,LCD_HIGHT,WHITE);
        }        
         os_free(pbuf);
	}
}
#include "md5.h"

static int func4(void *param)
{
	(void)param;
	for(;;){
		static ST_MD5 *md5;
		md5 = os_malloc(sizeof(ST_MD5));
		MD5_Handler(md5,(uint8_t *)"123",3);
		os_free(md5);		
        os_sleep(10);
	
	}
}

static int func5(void *param)
{
	(void)param;
    int show_type = 0;
	for(;;){
		char *pbuf = os_malloc(512);
		mutex_get(&mutex);
        if(++show_type % 10 >= 7){
            mem_info_little();
        }else{
            task_info_space(pbuf);
            lcd_drawstring(0,0,pbuf,RED,WHITE);
        }
		mutex_put(&mutex);
		os_free(pbuf);
		os_sleep(500);		
		
	}
}



int main(void)
{

    HAL_Init();
    MX_GPIO_Init();
    SystemClock_Config();      //32MHZ
    os_heartbeat(DISABLE);     //100HZ
    MX_USART2_UART_Init();     //115200-N-8-1
    MX_TIM7_Init();            //10000HZ IRQ
    MX_SPI1_Init();
    lcd_init();


    mutex_create(&mutex,2);
    q1 = queue_create(8,32);



    /*              STACK_SIZE             FUNC   PARAM  PRIO  NAME */
    task_create(DEFAULT_STACK_BYTE,(void *)func1,(void*)0, 1 ,"task1");
    task_create(DEFAULT_STACK_BYTE,(void *)func2,(void*)0, 3 ,"task2");
    task_create(DEFAULT_STACK_BYTE,(void *)func3,(void*)0, 3 ,"task3");
    task_create(DEFAULT_STACK_BYTE,(void *)func4,(void*)0, 4 ,"task4");
    task_create(DEFAULT_STACK_BYTE,(void *)func5,(void*)0, 4 ,"task5");

    os_start();

}


#include "dynamic_block.h"


#define SET_BIT_NEXT      (1)
#define SET_BIT_USED      (2)
#define RESET_BIT_MEM     (0)
#define GET_BIT_USED(x)   (x&2)
#define GET_BIT_NEXT(x)   (x&1)
#define CONFIG_MEM_BLOCK (CONFIG_HEAP_SIZE/CONFIG_HEAP_BLOCK)



struct mem_t{
	char *base;
	int allocate;
	int waterlevel;
	char flag[CONFIG_MEM_BLOCK];
};

static struct mem_t __mem = {0};
static  char __attribute((aligned (8))) __heap_buffer[CONFIG_HEAP_SIZE]; 


#include "st7789.h"

void mem_info(void)
{
	int length = 0;
	int c_index;
	int color = RED;
	int x = 0,y = 0;
	for(int i=0;i<CONFIG_MEM_BLOCK;i++){
		if(__mem.flag[i]){
			
			color = (c_index)?RED:BLUE;		
			lcd_drawchar_buffer(x,y,'~' +1,color,WHITE);
			if(!GET_BIT_NEXT(__mem.flag[i])){
				c_index = !c_index;
			}			
		}else{
			lcd_drawchar_buffer(x,y,'~' +1,GREEN,WHITE);
		}
		x += 8;
		if(x >240){
			x = 0;
			y+=16;
		}
	}
}

int get_free_block(void)
{
	return CONFIG_MEM_BLOCK - __mem.allocate;
}
int get_water_level(void)
{
	return CONFIG_MEM_BLOCK - __mem.waterlevel;
}

static void mem_init(void)
{
	__mem.allocate = 0;
	__mem.waterlevel = 0;
	__mem.base = __heap_buffer;
	
	for(int i=0;i<CONFIG_MEM_BLOCK;i++){
		__mem.flag[i] = RESET_BIT_MEM;
	}
}
__attribute__((weak)) void malloc_failed(void)
{
	//printf("not enough memory \n");
}

/* THIS = 7w/s  FreeRTOS = 5w/s @512Byte @32MHZ */
void *os_malloc(int length) 
{
	int idle = 0;
	int index = 0;
		
	os_enter_critical();
	
	while(__mem.base == 0){
		mem_init();
	}
	
	char *result = 0;
	int block = (length + CONFIG_HEAP_BLOCK -1)/CONFIG_HEAP_BLOCK;
	
	for(index = 0 ;index < CONFIG_MEM_BLOCK ; index++){
		if(!__mem.flag[index]){
			if((index + block) > (CONFIG_MEM_BLOCK)){
				
				break;
			}
			for(idle=1;idle<block;idle++){
				if(__mem.flag[index +idle] != 0) 
						break;
			}
			if(idle == block){
				for(idle = 0;idle < block-1 ;idle++){
					__mem.flag[idle + index] = SET_BIT_NEXT|SET_BIT_USED;
				}
				__mem.allocate += block;
				
				if(__mem.allocate > __mem.waterlevel)__mem.waterlevel = __mem.allocate;
				
				__mem.flag[index + idle] = SET_BIT_USED;
				result = __mem.base + (index * CONFIG_HEAP_BLOCK);
				break ;
			}else{
				index += idle;
			}			
		}		
	}	
	os_exit_critical();
	return result;
}
void os_free(void *point)
{
	if(point == 0)return;
	os_enter_critical();
	int index = ((char *)point - __mem.base)/CONFIG_HEAP_BLOCK;
	for(;GET_BIT_NEXT(__mem.flag[index]);index++){
		__mem.flag[index] = 0;
		__mem.allocate -- ;
	}
	__mem.allocate -- ;
	__mem.flag[index] = 0;
	os_exit_critical();
}



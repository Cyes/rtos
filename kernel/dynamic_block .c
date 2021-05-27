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
	char flag[CONFIG_MEM_BLOCK];
};

struct mem_t mem = {0};
static  char __heap_buffer[CONFIG_HEAP_SIZE]; 


int get_free_block(void)
{
	//return mem.allocate;
	return CONFIG_MEM_BLOCK - mem.allocate;
}

static void mem_init(void)
{
	mem.allocate = 0;
	mem.base = __heap_buffer;
	for(int i=0;i<CONFIG_MEM_BLOCK;i++){	
		mem.flag[i] = RESET_BIT_MEM;
	}
}

/* THIS = 7w/s  FreeRTOS = 5w/s @512Byte @32MHZ */
void *os_malloc(int length) 
{
	int idle = 0;
	int index = 0;
		
	os_enter_critical();
	
	while(mem.base == 0){
		mem_init();
	}
	
	char *result = 0;
	int block = (length + CONFIG_HEAP_BLOCK -1)/CONFIG_HEAP_BLOCK;
	
	for(index = 0 ;index < CONFIG_MEM_BLOCK ; index++){
		if(!mem.flag[index]){
			if((index + block) > (CONFIG_MEM_BLOCK)){
				break;
			}
			for(idle=1;idle<block;idle++){
				if(mem.flag[index +idle] != 0) 
						break;
			}
			if(idle == block){
				for(idle = 0;idle < block-1 ;idle++){
					mem.flag[idle + index] = SET_BIT_NEXT|SET_BIT_USED;
				}
				mem.allocate += block;
				mem.flag[index + idle] = SET_BIT_USED;
				result = mem.base + (index * CONFIG_HEAP_BLOCK);
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
	int index = ((char *)point - mem.base)/CONFIG_HEAP_BLOCK;
	for(;GET_BIT_NEXT(mem.flag[index]);index++){
		mem.flag[index] = 0;
		mem.allocate -- ;
	}
	mem.allocate -- ;
	mem.flag[index] = 0;
	os_exit_critical();
}



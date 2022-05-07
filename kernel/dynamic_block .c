#include "dynamic_block.h"





#define SET_BIT_NEXT      (1)
#define SET_BIT_USED      (2)
#define RESET_BIT_MEM     (0)

//malloc 128 bytes
//flag: [3] [3] [3] [2] [0]
//3 = SET_BIT_NEXT | SET_BIT_USED





static struct mem_t __mem = {0};
static char __attribute((aligned (CONFIG_HEAP_BLOCK))) __heap_buffer[CONFIG_HEAP_SIZE]; 

inline uint32_t align(uint32_t num,uint32_t nbyte)
{
    return ( num + nbyte -1 ) & ( ~(uint32_t)(nbyte -1) );
}


const void *get_mem_desc(void)
{
	return (void *)&__mem;
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
    os_enter_critical();
	__mem.allocate = 0;
	__mem.waterlevel = 0;
	__mem.base = __heap_buffer;
	
	for(int i=0;i<CONFIG_MEM_BLOCK;i++){
		__mem.flag[i] = RESET_BIT_MEM;
	}
    os_exit_critical();
}

__attribute__((weak)) void malloc_failed(void)
{
	//printf("not enough memory \n");
}


void *os_malloc(int length) 
{
		
	os_enter_critical();
	
	int idle = 0;
	int index = 0;
	
	while(__mem.base == NULL){
		mem_init();
	}
	
	char *result = NULL;
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
	if(point == NULL)return;
    void *pstart = __mem.base;
    void *pend = __mem.base + CONFIG_HEAP_SIZE;

    if((point < pstart) || (point >= pend)){
        return ;
    }
    
    if(((int)point) & (CONFIG_HEAP_BLOCK -1)){
        return ;
    }
    
    
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


void *os_realloc(void *ptr, int length)
{
    void *pnew = os_malloc(length);
    if(NULL != pnew){
        memcpy(pnew,ptr,length);
        os_free(ptr);
    }
    return pnew;
}


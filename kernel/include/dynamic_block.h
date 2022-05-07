#ifndef __DYNAMEIC_BLOCK_H__
#define __DYNAMEIC_BLOCK_H__

#include "kernel.h"

#define GET_BIT_USED(x)   (x&2)
#define GET_BIT_NEXT(x)   (x&1)


struct mem_t{
	char *base;
	int allocate;
	int waterlevel;
	char flag[CONFIG_MEM_BLOCK];
};

void os_free(void *p);
const void *get_mem_desc(void);
int get_free_block(void);
int get_water_level(void);
void *os_malloc(int length);
uint32_t align(uint32_t num,uint32_t nbyte);






#endif

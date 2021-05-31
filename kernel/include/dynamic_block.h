#ifndef __DYNAMEIC_BLOCK_H__
#define __DYNAMEIC_BLOCK_H__

#include "kernel.h"
void mem_info(void);





void os_free(void *p);
void *os_malloc(int length);
int get_free_block(void);
int get_water_level(void);






#endif

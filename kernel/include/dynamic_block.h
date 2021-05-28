#ifndef __DYNAMEIC_BLOCK_H__
#define __DYNAMEIC_BLOCK_H__

#include "kernel.h"




#define CONFIG_HEAP_SIZE  (1024 * 40)
#define CONFIG_HEAP_BLOCK (128)

void os_free(void *p);
void *os_malloc(int length);
int get_free_block(void);
int get_water_level(void);






#endif

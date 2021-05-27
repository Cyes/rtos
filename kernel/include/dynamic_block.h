#ifndef __DYNAMEIC_BLOCK_H__
#define __DYNAMEIC_BLOCK_H__

#include "kernel.h"




#define CONFIG_HEAP_SIZE  (1024 * 20)
#define CONFIG_HEAP_BLOCK (512)

void os_free(void *p);
void *os_malloc(int length);
int get_free_block(void);






#endif

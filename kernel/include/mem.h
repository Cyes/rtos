#ifndef __MEM_H__
#define __MEM_H__

#include "kernel.h"

void *os_malloc(int);
void os_free(void *);
void mem_init(void);


#endif

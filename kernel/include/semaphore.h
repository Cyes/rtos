#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include "kernel.h"

struct semaphore_t{
	int max_val;
	int counter;
	struct task_list_t put,get;
};

int semaphore_get(struct semaphore_t *sem);
int semaphore_put(struct semaphore_t *sem);
void semaphore_create(struct semaphore_t *sem, int initial_val, int max_val);

#endif
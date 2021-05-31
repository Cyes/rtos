#ifndef __MUTEX_H__
#define __MUTEX_H__
#include "kernel.h"

struct mutex_t{
	int lock;
	int prio;
	struct task_desc_t *owner;
	struct list_head get,put;
};


int mutex_get(struct mutex_t *mux);
int mutex_put(struct mutex_t *mux);
void mutex_create(struct mutex_t *mux, int prio);

#endif
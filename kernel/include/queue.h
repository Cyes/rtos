#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "kernel.h"






struct queue_t{
	char *memery;
	int free;
    int *psize;
	int write,read;
	int size,count;
	int txlock,rxlock;
	struct task_list_t tx,rx;
};


struct queue_t *queue_create(int count, int size);
int queue_read(struct queue_t *q, char *buffer, int tick);
int queue_write(struct queue_t *q, const char *buffer, int length, int tick);

int queue_isfull(struct queue_t *q);
int queue_isempty(struct queue_t *q);

#endif

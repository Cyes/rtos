#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "kernel.h"

struct queue_t{
	char *memery;
	int message;
	int write,read;
	int size,count;
	int txlock,rxlock;
	struct list_head task_tx;
	struct list_head task_rx;
};

int queue_reset(struct queue_t *q, int count, int size, void *memery);
int queue_read(struct queue_t *q, char *buffer, int length, int tick);
int queue_write(struct queue_t *q, const char *buffer, int length, int tick);

#endif

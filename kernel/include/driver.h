#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "kernel.h"

struct fops_t{
	int (*close)(int id);
	int (*open)(void *name);
	int (*read)(int id, char *buffer, int length);
	int (*write)(char *buffer, int length);
	int (*ioctl)(int id, void *parma);
};

struct dev_t{
	char name[8];
	int *param;
	int lock;
	int priority;
	struct fops_t io;
	struct list_head list;
};

int dev_open(char *name);
int dev_close(char *name);
void dev_create(struct dev_t *dev,struct fops_t *fops,char *name, void *param);


#endif





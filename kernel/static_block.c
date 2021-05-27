#include "mem.h"



#define MEM_BLOCK_SIZE_01  1024
#define MEM_BLOCK_BLCK_01  16     //=16 not work
#define MEM_BLOCK_SIZE_02  0
#define MEM_BLOCK_BLCK_02  0

#define MEM_BLOCK_SIZE_03  0
#define MEM_BLOCK_BLCK_03  0
#define MEM_BLOCK_SIZE_04  0
#define MEM_BLOCK_BLCK_04  0


struct  mem_node_t{
	char *base;
	int  magic;
	struct list_head list;
};

struct mem_desc_t{
	char *base;
	int size;
	int block;
	int req_cnt;
	struct mem_node_t *node;
	struct list_head free;
	struct list_head used;
	struct list_head list;
};


#if MEM_BLOCK_BLCK_01
__attribute((aligned (8))) static struct mem_node_t __nd_01[MEM_BLOCK_BLCK_01];
__attribute((aligned (8))) static char __mem_base_01[MEM_BLOCK_BLCK_01 *MEM_BLOCK_SIZE_01];

static struct mem_desc_t __md_01 = {
	.base = __mem_base_01,
	.size = MEM_BLOCK_SIZE_01 ,
	.block = MEM_BLOCK_BLCK_01 ,
	.node = __nd_01,
	
};
#endif

#if MEM_BLOCK_BLCK_02
__attribute((aligned (8))) static struct mem_node_t __nd_02[MEM_BLOCK_BLCK_02];
__attribute((aligned (8))) static char __mem_base_02[MEM_BLOCK_BLCK_02 *MEM_BLOCK_SIZE_02];
static struct mem_desc_t __md_02 = {
	.base = __mem_base_02,
	.size = MEM_BLOCK_SIZE_02 ,
	.block = MEM_BLOCK_BLCK_02 ,
	.node = __nd_02,
	
};
#endif

#if MEM_BLOCK_BLCK_03
__attribute((aligned (8))) static struct mem_node_t __nd_03[MEM_BLOCK_BLCK_03];
__attribute((aligned (8))) static char __mem_base_03[MEM_BLOCK_BLCK_03 *MEM_BLOCK_SIZE_03];
static struct mem_desc_t __md_03 = {
	.base = __mem_base_03,
	.size = MEM_BLOCK_SIZE_03,
	.block = MEM_BLOCK_BLCK_03,
	.node = __nd_03,
	
};
#endif

#if MEM_BLOCK_BLCK_04
__attribute((aligned (8))) static struct mem_node_t __nd_04[MEM_BLOCK_BLCK_04];
__attribute((aligned (8))) static char __mem_base_04[MEM_BLOCK_BLCK_04 *MEM_BLOCK_SIZE_04];
static struct mem_desc_t __md_04 = {
	.base = __mem_base_04,
	.size = MEM_BLOCK_SIZE_04,
	.block = MEM_BLOCK_BLCK_04,
	.node = __nd_04,
	
};
#endif



LIST_HEAD(g_memery);



void mem_create(struct mem_desc_t  *md, struct  mem_node_t *nd)    //必须从小到大创建分区
{
	struct  mem_node_t *node = NULL;
	
	INIT_LIST_HEAD(&md->free);
	INIT_LIST_HEAD(&md->used);
	INIT_LIST_HEAD(&md->list);
	//printf("create base = %p  size = %d  block = %d\n",md->base,md->size,md->block);
	
	for(int i=0;i<md->block;i++){
		node = nd + i ;
		INIT_LIST_HEAD(&node->list);
		node->base = md->base +( md->size * i );
		list_add_tail(&node->list, &md->free );
		//printf("node[%d] = %p\n",i,node->base);
	}

	list_add_tail( &md->list, &g_memery );
	//printf("\n");
}


void mem_init(void)
{
#if MEM_BLOCK_BLCK_01
	mem_create(&__md_01,__nd_01);
#endif
#if MEM_BLOCK_BLCK_02
	mem_create(&__md_02,__nd_02);
#endif
#if MEM_BLOCK_BLCK_03
	mem_create(&__md_03,__nd_03);
#endif
#if MEM_BLOCK_BLCK_04
	mem_create(&__md_04,__nd_04);
#endif
}


void *os_malloc(int  size)
{
	void *memery = NULL;
	struct list_head *pos,*temp;
	struct mem_desc_t *pmd;
	struct mem_node_t *pnd;
	//printf("malloc ( %d )\n",size);
	os_enter_critical();
	if(list_empty(&g_memery)){
		mem_init();
	}

	list_for_each_safe(pos,temp,&g_memery){
		pmd = list_entry( pos,struct mem_desc_t, list );
		if(( pmd->size >=size ) && ( !list_empty(&pmd->free)) ){    //找到md并且有空间	

			//printf("find block = %d\n",pmd->size);    //找到合适大小内存分区
			pnd = list_entry(pmd->free.next,struct mem_node_t,list);		
			list_move( &pnd->list, &pmd->used);	
			memery = pnd->base;
			pmd->req_cnt ++;
			break;
		}
	}
	//printf( "return %p\n\n",memery );
	os_exit_critical();
	return memery;
}

void os_free(void *memery)
{
	int node_index = 0;
	struct list_head *pos,*temp;
	struct mem_desc_t *pmd;
	struct  mem_node_t *node;
	
	os_enter_critical();
	//printf("free ( %p )\n",memery);
	list_for_each_safe(pos,temp,&g_memery){
		
		pmd = list_entry(pos,struct mem_desc_t,list);		
		uint32_t obj = (uint32_t) memery;
		uint32_t sta = (uint32_t) pmd->base;
		uint32_t end = sta +(pmd->size * pmd->block);
		node_index = (obj - sta) / (pmd->size);
		
		if((obj >= sta)&&(obj < end)){
			node = pmd->node + node_index;
			if(node->base == memery)			
				list_move_tail(&node->list, &pmd->free);			
		}
	}
	os_exit_critical();
}





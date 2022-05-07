#include "kernel.h"



#define MAX_PRIO_NUMBER           (32)//fixed
#define SYSTEM_TICK_FREQ          (100U)
#define DEFAULT_STACK_BYTE        (1024)
#define CONFIG_HEAP_SIZE          (1024 * 32 + 384)
#define CONFIG_HEAP_BLOCK         (32)
#define CONFIG_MEM_BLOCK (CONFIG_HEAP_SIZE/CONFIG_HEAP_BLOCK)
#define CONFIG_STACK_CHECK


//#define CONFIG_USE_TIMER  1

#ifdef CONFIG_USE_TIMER
#define TIMER_STACK_BYTE          (512)
#define TIMER_TASK_PRIO           (10)
#endif


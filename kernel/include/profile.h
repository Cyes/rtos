#include "kernel.h"



#define MAX_PRIO_NUMBER           (32)//fixed
#define SYSTEM_TICK_FREQ          (1000U)
#define DEFAULT_STACK_BYTE        (1024)
#define CONFIG_HEAP_SIZE          (1024 * 42 - 1344)
#define CONFIG_HEAP_BLOCK         (168)


#ifdef CONFIG_USE_TIMER
#define CONFIG_USE_TIMER
#define TIMER_STACK_BYTE          (1024)
#define TIMER_TASK_PRIO           (10)
#endif


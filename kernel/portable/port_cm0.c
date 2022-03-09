#include "port.h"

//utf-8,unix(LF)
//only support mdk compiler v6
//this is cortex-m0,m0+ portable file


void set_pendsv(void)
{
	SCB->ICSR |= 1<< 28;
}

void os_disable_irq(void)
{
	__asm volatile("    cpsid i    ");
}
void os_enable_irq(void)
{
	__asm volatile("    cpsie i    ");
}

void set_systick(int status)
{
	if(status){
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//1UL
	}else{
		SysTick->CTRL &= ~ (SysTick_CTRL_ENABLE_Msk);
	}

}
void SysTick_Handler(void)
{
	extern __IO uint32_t uwTick;
		
	uwTick ++;
	os_inc_tick();
}

void os_svc(void)
{
	__asm volatile("    svc 0    ");
}



void PendSV_Handler(void)
{	
    
    //task structure keeping aligned ( 9 REG ,REG = 4 byte)
    //save the current environment, push 8 REG, but task stack = stack - 9 REG
    //get next environment, pop 8 REG, but task stack = stack + 9 REG

    __asm volatile (
        
    /*task stack structure is 9 byte. so, push & pop need 9 REG*/
    /*cortex-m0 instruction too few*/
    "    dsb                          \n"                         
    "    isb                          \n"
    
    //push current context,save stack = psp - 9 REG
    "    mrs r0, psp                  \n"
    "    subs r0, #36                 \n"
    "    ldr r3, task_psv             \n"          
    "    ldr r2, [r3]                 \n"
    "    str r0, [r2]                 \n"
    
    //cortex-m0 no 'stmdb'
    //continue to push {r4-r11},and reserve a place for r14
    "    stmia r0!, {r4-r7}           \n"
    "    mov r4, r8	  	              \n"		
    "    mov r5, r9                   \n"
    "    mov r6, r10                  \n"
    "    mov r7, r11                  \n"
    "    stmia r0!, {r4-r7}           \n"

    //find next contxt
    "    push {r3,r14}                \n"
    "    cpsid i                      \n"           
    "    bl next_context              \n"         
    "    cpsie i                      \n"          
    "    pop {r2,r3}                  \n"   
    //now,  r3=0xFFFFFFFD, r2 = currentTD
    
    //pop next context
    //got next stack pointer
    "    ldr r1, [r2]                 \n"
    "    ldr r0, [r1]                 \n"   
    
    //pop {r4-r11}
    "    adds r0, # 16                \n"
    "    ldmia r0!, {r4-r7}           \n"
    "    mov r8, r4                   \n"
    "    mov r9, r5                   \n"
    "    mov r10, r6                  \n"
    "    mov r11, r7                  \n"
    
    //update stack = r0 +8 REG +1 REG(reserve r14)
    "    adds r0, #4                  \n"
    "    msr psp, r0                  \n"
    "    str r0, [r1]                 \n"
    "    subs r0, #36                 \n"
    "    ldmia r0!, {r4-r7}           \n"
    
    "    bx r3                        \n"
    "    .align 4                     \n"
    "    task_psv: .word currentTD    \n"

    );

}


void SVC_Handler(void)
{
    __asm volatile (
        //use to run first tash,init stack
        //got current task stack pointer
        //Pretend pop 9 REG, update psp
        "    ldr r1, task_svc             \n"         
        "    ldr r1, [r1]                 \n"        
        "    ldr r2, [r1]                 \n"       
        "    adds r2, #36                 \n"       
        "    msr psp, r2                  \n"      
        //set lr = 0xFFFFFFFD
        "    mov r0, lr                   \n"     
        "    movs r3, #4                  \n"      
        "    orrs r0, r0, r3              \n"     
        "    mov lr, r0                   \n"    
        
        "    bx r14                       \n" 
        "    .align 4                     \n"
        "    task_svc: .word currentTD    \n"
    );   
}






#include "port.h"

#ifdef __GNUC__

void SysTick_Handler(void)
{
	os_inc_tick();
}
void os_disable_irq(void)
{
	__asm volatile("    cpsid i    ");
}
void os_enable_irq(void)
{
	__asm volatile("    cpsie i    ");
}

void os_svc(void)
{
	__asm volatile("    svc 0      ");
}

void SVC_Handler(void)
{
	__asm volatile (
									"    ldr r1, task_svc          \n"
									"    ldr r1, [r1]              \n"
									"    ldr r2, [r1]              \n"
									"    add r2, #36               \n"
									"    msr psp, r2               \n"
									"                              \n"
									"    orr lr, lr, #0x04         \n"
									"    bx r14                    \n"
									"    .align 4                  \n"
                  "    task_svc: .word currentTD \n"		
							 );
}



void PendSV_Handler(void)
{	
	__asm volatile (
									"    dsb                       \n"
									"    isb                       \n"
									"    mrs r0, psp               \n"
									"    tst r14, #0x10            \n"
									"    it eq                     \n"
									"    vstmdbeq r0!, {s16-s31}   \n"
									"    stmdb r0!, {r4-r11,r14}   \n"
                  "    ldr r3, task_psv          \n"
									"    ldr r2, [r3]              \n"
									"    str r0, [r2]              \n"
									"                              \n"
									"    push {r3,r14}             \n"
									"    cpsid i                   \n"
									"    bl next_context           \n"
									"    cpsie i                   \n"
									"    pop {r3,r14}              \n"
									"                              \n"
									"    ldr r1, [r3]              \n"
									"    ldr r0, [r1]              \n"
									"    ldmia r0!, {r4-r11,r14}   \n"
									"    tst r14, #0x10            \n"
									"    it eq                     \n"
									"    vldmiaeq r0!, {s16-s31}   \n"
									"    msr psp, r0               \n"
									"    isb                       \n"
									"    str r0, [r1]              \n"
									"    orr lr, lr, #0x04         \n"
									"    dsb                       \n"
									"    isb                       \n"
									"                              \n"
									"    bx r14                    \n"
									"    .align 4                  \n"
									"    task_psv: .word currentTD \n"
	);
	
}

#endif


#ifdef __CC_ARM

__asm void PendSV_Handler(void)
{	
	PRESERVE8
	extern currentTD
	extern next_context
	
	dsb
	isb
	mrs r0, psp
	tst r14, #0x10
	it eq
	vstmdbeq r0!, {s16-s31}
	stmdb r0!, {r4-r11,r14}
	ldr	r3, =currentTD
	ldr	r2, [r3]
	str r0, [r2]
	
	push {r3,r14}
	bl next_context
	pop {r3,r14}
	
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11,r14}
	tst r14, #0x10
	it eq
	vldmiaeq r0!, {s16-s31}
	msr psp, r0
	isb
	str r0, [r1]
	orr lr, lr, #0x04
	dsb
	isb
	
	bx r14
	
}


__asm void SVC_Handler(void)
{
	extern currentTD
	
	ldr	r1, =currentTD
	ldr r1, [r1]
	ldr r2, [r1]
	add r2, #36
	msr psp, r2
	
	orr lr, lr, #0x04
	bx r14
}


#endif 



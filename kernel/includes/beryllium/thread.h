#ifndef THREAD_T
#define THREAD_T

#include <types.h>

#define THREAD_LEVEL_KERNEL 0
#define THREAD_LEVEL_USER 3

#define THREAD_STACKSIZE 0x1000 ///Allocate one page

typedef volatile struct
{
	uint32_t esp, ebp, ebx, esi, edi, eflags;
	uint32_t tid;
	uint8_t level; /// Follows x86 Rings
	uint32_t owner_pid;
	uint32_t * stack;
	uint32_t * stack_ptr;
	uint32_t status;
	
} thread_t;

void thread_exit();
thread_t * thread_create(uint8_t level,uint32_t pid, int (*fn)(void*));
thread_t *threading_start();
extern void thread_switch (thread_t *next);
void thread_switchkernel();

#endif

#ifndef __PROCESS__
#define __PROCESS__
#include "clock.h"
#include "mem.h"
#include "queue.h"


#include "processor_structs.h"

#define NBPROC 1200
#define MAXPRIO 0x100

extern int pidc;
extern int first_stack[];
extern struct process pidle, *running_proc;
extern link ready_head_list;

void ctx_sw(void);

enum State { RUNNING, READY, ZOMBIE, SEMAPHORE_BLOCKED, IO_BLOCKED, CHILD_BLOCKED, SUSPENDED, BLOCKED_ON_FULL_QUEUE, BLOCKED_ON_EMPTY_QUEUE, DEAD };

struct page_frame {
    unsigned status, priority;
    void *physical;
    link mainlink;
};

struct process {
    unsigned priority, waketime, ssize, status, *pgdir;
    int pid, mqd, registers[9], sere, sudo, nbpages, *message, *stack, *sustack, idSem, idWake;
    char name[16];
    link mainlink, childlink, childs_list, bin_page_list, heap_page_list, stack_page_list;
    struct process *parent;
    struct page_frame *pg_dir, *bin_pg_tab, *heap_pg_tab, *stack_pg_tab;
};

#if (!VM)
int start(int (*pt_func)(void*), unsigned long ssize, int prio, const char *name, void *arg);
#endif

void process_info(void);

void shm_info(void);

void sys_exit(void);

void exit(int retval);

int kill(int pid);

int getprio(int pid);

int chprio(int pid, int newprio);

int waitpid(int pid, int *retvalp);

int getpid(void);

int idle(void * arg);

void init(void);

int test1(void *arg);

int test2(void *arg);

int test3(void *arg);

int test4(void *arg);

int test5(void *arg);

int test6(void *arg);

int test7(void *arg);

int test8(void *arg);

int test10(void *arg);

int test12(void *arg);

int test13(void *arg);

int test14_msg(void *arg);

int test14_sem(void *arg);

int test15(void *arg);

int test16(void *arg);

int test17(void *arg);

int test21(void *arg);

int test22(void *arg);

#endif

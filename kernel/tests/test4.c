#include "process.h"

#ifdef microblaze
static const int loop_count0 = 500;
static const int loop_count1 = 1000;
#else
static const int loop_count0 = 5000;
static const int loop_count1 = 10000;
#endif


void test_it(void)
{
#ifdef microblaze
    int status, mstatus;
    __asm__ volatile("mfs\t%0, rmsr\n\tori %1, %0, 2\n\tmts\trmsr, %1\n\tnop\n\tnop\n\tmts\trmsr, %0" : "=r" (status), "=r" (mstatus));
#else
    __asm__ volatile("pushfl\n\ttestl\t$0x200, (%%esp)\n\tjnz\t0f\n\tsti\n\tnop\n\tcli\n0:\n\taddl\t$4, %%esp" ::: "memory");
#endif
}


int busy1(void *arg)
{
    (void)arg;
    for (;;)
    {
        int i, j;

        printf(" A");
        for (i = 0; i < loop_count1; i++)
        {
            test_it();
            for (j = 0; j < loop_count0; j++);
        }
    }
    return 0;
}


int busy2(void *arg)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        int k, j;

        printf(" B");
        for (k = 0; k < loop_count1; k++)
        {
            test_it();
            for (j = 0; j < loop_count0; j++);
        }
    }
    i = chprio((int) arg, 16);
    assert(i == 64);
    return 0;
}


int test4(void *args)
{
    int pid1 = -1, pid2 = -1;
    int r;
#if (!VM)
    int arg = 0;
#endif
    (void)args;

    assert(getprio(getpid()) == 128);
#if (!VM)
    pid1 = start(busy1, 4000, 64, "busy1", (void *)arg);
#endif
    assert(pid1 > 0);
#if (!VM)
    pid2 = start(busy2, 4000, 64, "busy2", (void *)pid1);
#endif
    assert(pid2 > 0);
    printf("1 -");
    r = chprio(getpid(), 32);
    assert(r == 128);
    printf(" - 2");
    r = kill(pid1);
    assert(r == 0);
    assert(waitpid(pid1, 0) == pid1);
    r = kill(pid2);
    assert(r < 0); /* kill d'un processus zombie */
    assert(waitpid(pid2, 0) == pid2);
    printf(" 3");
    r = chprio(getpid(), 128);
    assert(r == 32);
    printf(" 4.\n");
    return 0;
}

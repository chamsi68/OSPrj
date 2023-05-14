#include "process.h"


int procExit(void *args)
{
    printf(" 5");
    exit((int) args);
    assert(0);
    return 0;
}

int procKill(void *args)
{
    printf(" X");
    return (int)args;
}


int test2(void *arg)
{
    int rval;
    int r;
    int pid1 = -1;
    int val = 45;

    (void)arg;

    printf("1");
#if (!VM)
    pid1 = start(procKill, 4000, 100, "procKill", (void *)val);
#endif
    assert(pid1 > 0);
    printf(" 2");
    r = kill(pid1);
    assert(r == 0);
    printf(" 3");
    r = waitpid(pid1, &rval);
    assert(rval == 0);
    assert(r == pid1);
    printf(" 4");
#if (!VM)
    pid1 = start(procExit, 4000, 192, "procExit", (void *)val);
#endif
    assert(pid1 > 0);
    printf(" 6");
    r = waitpid(pid1, &rval);
    assert(rval == val);
    assert(r == pid1);
    assert(waitpid(getpid(), &rval) < 0);
    printf(" 7.\n");
    return 0;
}

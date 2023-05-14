#include "process.h"


#if defined microblaze
__asm__(
    "\t.text\n"
    "\t.globl\tproc6_1\n"
    "proc6_1:\n"
    "\taddik\tr3, r0, 3\n"
    "\trtsd\tr15, 8\n"
    "\tnop\n"
    ".previous\n"
);
#else
__asm__(
    "\t.text\n"
    "\t.globl\tproc6_1\n"
    "proc6_1:\n"
    "\tmovl\t$3, %eax\n"
    "\tret\n"
    ".previous\n"
);
#endif

#if defined microblaze
__asm__(
    "\t.text\n"
    "\t.globl\tproc6_2\n"
    "proc6_2:\n"
    "\taddk\tr3, r0, r5\n"
    "\tswi\tr3, r1, -4\n"
    "\trtsd\tr15, 8\n"
    "\tnop\n"
    ".previous\n"
);
#else
__asm__(
    "\t.text\n"
    "\t.globl\tproc6_2\n"
    "proc6_2:\n"
    "\tmovl\t4(%esp), %eax\n"
    "\tpushl\t%eax\n"
    "\tpopl\t%eax\n"
    "\tret\n"
    ".previous\n"
);
#endif

#if defined microblaze
__asm__(
    "\t.text\n"
    "\t.globl\tproc6_3\n"
    "proc6_3:\n"
    "\taddk\tr3, r0, r5\n"
    "\tswi\tr3, r1, -4\n"
    "\trtsd\tr15, 8\n"
    "\tnop\n"
    ".previous\n"
);
#else
__asm__(
    "\t.text\n"
    "\t.globl\tproc6_3\n"
    "proc6_3:\n"
    "\tmovl\t4(%esp), %eax\n"
    "\tpushl\t%eax\n"
    "\tpopl\t%eax\n"
    "\tret\n"
    ".previous\n"
);
#endif

int proc6_1(void *arg);
int proc6_2(void *arg);
int proc6_3(void *arg);


int test6(void *arg)
{
    int pid1 = -1, pid2 = -1, pid3 = -1;
    int ret;

    (void)arg;

    assert(getprio(getpid()) == 128);
#if (!VM)
    pid1 = start(proc6_1, 0, 64, "proc6_1", 0);
#endif
    assert(pid1 > 0);
#if (!VM)
    pid2 = start(proc6_2, 4, 66, "proc6_2", (void *)4);
#endif
    assert(pid2 > 0);
#if (!VM)
    pid3 = start(proc6_3, 0xffffffff, 65, "proc6_3", (void *)5);
#endif
    assert(pid3 < 0);
#if (!VM)
    pid3 = start(proc6_3, 8, 65, "proc6_3", (void *)5);
#endif
    assert(pid3 > 0);
    assert(waitpid(-1, &ret) == pid2);
    assert(ret == 4);
    assert(waitpid(-1, &ret) == pid3);
    assert(ret == 5);
    assert(waitpid(-1, &ret) == pid1);
    assert(ret == 3);
    assert(waitpid(pid1, 0) < 0);
    assert(waitpid(-1, 0) < 0);
    assert(waitpid(getpid(), 0) < 0);
    printf("ok.\n");
    return 0;
}

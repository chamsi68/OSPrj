#include "sh_mem.h"

#define NB_PROCS 10

struct tst16 {
    int count, fid;
};

void test_it(void);


int proc16_1(void *arg)
{
    struct tst16 *p = NULL;
    int i, msg;

    (void)arg;
    p = shm_acquire("test16_shm");
    assert(p != NULL);

    for (i = 0; i <= p->count; i++) {
        assert(preceive(p->fid, &msg) == 0);
        assert(msg == i);
        test_it();
    }
    shm_release("test16_shm");
    return 0;
}


int proc16_2(void *arg)
{
    struct tst16 *p = NULL;
    int i, msg;

    (void)arg;
    p = shm_acquire("test16_shm");
    assert(p != NULL);

    for (i = 0; i < p->count; i++) {
        assert(preceive(p->fid, &msg) == 0);
        test_it();
    }
    shm_release("test16_shm");
    return 0;
}


int proc16_3(void *arg)
{
    struct tst16 *p = NULL;
    int i;

    (void)arg;
    p = shm_acquire("test16_shm");
    assert(p != NULL);

    for (i = 0; i < p->count; i++) {
        assert(psend(p->fid, i) == 0);
        test_it();
    }
    shm_release("test16_shm");
    return 0;
}


int test16(void *arg)
{
    int i, count, fid, pid = -1;
    struct tst16 *p = NULL;
    int pids[2 * NB_PROCS];

    (void)arg;
    p = (struct tst16*) shm_create("test16_shm");
    assert(p != NULL);

    assert(getprio(getpid()) == 128);
    for (count = 1; count <= 100; count++) {
        fid = pcreate(count);
        assert(fid >= 0);
        p->count = count;
        p->fid = fid;
#if (!VM)
        pid = start(proc16_1, 2000, 128, "proc16_1", 0);
#endif
        assert(pid > 0);
        for (i = 0; i <= count; i++) {
            assert(psend(fid, i) == 0);
            test_it();
        }
        assert(waitpid(pid, 0) == pid);
        assert(pdelete(fid) == 0);
    }

    p->count = 20000;
    fid = pcreate(50);
    assert(fid >= 0);
    p->fid = fid;
    for (i = 0; i< NB_PROCS; i++) {
#if (!VM)
        pid = start(proc16_2, 2000, 127, "proc16_2", 0);
#endif
        assert(pid > 0);
        pids[i] = pid;
    }
    for (i=0; i < NB_PROCS; i++) {
#if (!VM)
        pid = start(proc16_3, 2000, 127, "proc16_3", 0);
#endif
        assert(pid > 0);
        pids[NB_PROCS + i] = pid;
    }
    for (i = 0; i < 2 * NB_PROCS; i++)
        assert(waitpid(pids[i], 0) == pids[i]);
    assert(pcount(fid, &count) == 0);
    assert(count == 0);
    assert(pdelete(fid) == 0);

    shm_release("test16_shm");
    printf("ok.\n");
    return 0;
}

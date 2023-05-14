#include "sh_mem.h"

#define FILL_VALUE          0xCA
#define CHECKER_SUCCESS     0x12345678


int shm_checker(void *arg)
{
    (void)arg;
    char *shared_area = NULL;

    shared_area = shm_acquire("test21-shm");
    assert(shared_area != NULL);

    /* Check we get the memory filled by the main process */
    for (int i = 0; i < 4096; i++)
        if (shared_area[i] != (char)FILL_VALUE)
            return -1;

    /*
    * Fill it with something else to let the main process check we success
    * to access it.
    */
    memset(shared_area, 0, 4096);

    return (int)CHECKER_SUCCESS;
}


int test21(void *arg)
{
    (void)arg;
    char *shared_area = NULL;
    int checker_pid = -1;
    int checker_ret = -1;

    printf("\n%s\n", "Test 21: checking shared memory space ...");

    shared_area = shm_create("test21-shm");
    assert(shared_area != NULL);

    /* We have to be able to fill at least 1 page */
    memset(shared_area, FILL_VALUE, 4096);

    /* Let the check do its job */
#if (!VM)
    checker_pid = start(shm_checker, 4000, getprio(getpid()) - 1, "shm_checker", NULL);
#endif
    assert(checker_pid > 0);

    waitpid(checker_pid, &checker_ret);

    switch (checker_ret) {
    case CHECKER_SUCCESS:
        printf(" -> %s\n -> %s\n", "\"shm_checker\" ends correctly.", "TEST PASSED");
        break;
    case 0:
        printf(" -> %s\n -> %s\n", "\"shm_checker\" killed.", "TEST FAILED");
        break;
    default:
        printf(" -> %s\n -> %s\n", "\"shm_checker\" returned inconsistent value. Check waitpid implementation.", "TEST FAILED");
    }

    int shm_valid = 1;
    for (int i = 0; i < 4096; i++)
        if (shared_area[i] != 0)
            shm_valid = 0;

    if (shm_valid)
        printf(" -> %s\n -> %s\n", "shm area content is correct.", "TEST PASSED");
    else
        printf(" -> %s\n -> %s\n", "shm area content is invalid.", "TEST FAILED");

    shm_release("test21-shm");
    return 0;
}

#include "sysapi.h"


int main(void *arg)
{
    wait_clock(current_clock() + 50 * (unsigned long)arg);
    return 0;
}

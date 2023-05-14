#include "sysapi.h"


int main(void *arg)
{
    if (!arg)
        sys_info(1);
    else if ((int)arg == 35)
        sys_info(2);
    else if ((int)arg == 31)
        sys_info(3);
    else if ((int)arg == 37)
        sys_info(4);
    else
    {
        printf("ipcs: invalid option -- '%s'\n", arg);
        return 1;
    }
    return 0;
}

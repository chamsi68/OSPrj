#include "sysapi.h"


int main(void *arg)
{
    return kill((int)arg);
}

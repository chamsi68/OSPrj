#include "sysapi.h"


int main(void *arg)
{
    if (!arg)
	    printf("These commands are defined internally.\nType 'help' to see this list.\nType 'ipcs' to display message queue [-q], shared memory [-m] and semaphore [-s] informations.\nType 'ps' to display the currently-running processes.\nType 'echo' to activate/desactivate echo.\nType 'exit' to exit the shell.\n");
    else
        printf("help: invalid option -- '%s'\n", arg);
    return 0;
}

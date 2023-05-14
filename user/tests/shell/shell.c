#include "sysapi.h"

int echo = 1, power10[10] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };


void *stringtoint(char *string)
{
	int result = 0;
	unsigned index, len;
	if (!string)
		return 0;
	len = strlen(string);
	for (index = 0; index < len; index++)
		result += power10[len - 1 - index] * (string[index] - 48);
	return (void *)result;
}


void fork(int argc, char *argv[])
{
	int pid, background = 0;
	if (!strcmp(argv[argc - 1], "&"))
		background++;
	if (!strcmp(*argv, "echo"))
	{
		echo = !echo;
		pid = start(*argv, 8180, getprio(getpid()), (void *)echo);
	}
	else
		pid = start(*argv, 8180, getprio(getpid()), stringtoint(argv[1]));
	if (pid < 0)
		printf("Command '%s' not found.\n", *argv);
	else if (!background)
		waitpid(pid, NULL);
}


int main(void *arg)
{
	int argc;
	char command[256], *argv[16] = { 0 }, *token;
	(void)arg;
	printf("Team: CHAMSI Mohamed Hadi, HRARTI Zakaria, AIT TALEB Aymane, MESTASSI Abdelouahab, GRAINI Bassam, ASLOUNE Salaheddine.\n");
	printf("Type 'help' to display information on available commands.\n");
	for (;;)
	{
		printf("\nuser@NyanOS:~$");
		if (!cons_read(command, 256))
			continue;
		token = strtok(command, " ");
		while (token)
		{
			if (*token)
				argv[argc++] = token;
			token = strtok(0, " ");
		}
		if (!strcmp(*argv, "exit"))
			break;
		fork(argc, argv);
		argc = 0;
		memset(argv, 0, 16 * sizeof(void *));
		memset(command, 0, 256);
	}
	return 0;
}

#include "keyboard-glue.h"
#define min(x, y) ((int)(x) < (int)(y) ? (int)(x) : (int)(y))

int stdin, echo = true, index = 0, users = 0;

void cons_echo(int on)
{
	if (!on)
		echo = false;
	else
		echo = true;
}


void write_echo(char c)
{
	if (echo)
	{
		if (c == 9 || (c >= 32 && c <= 126))
			printf("%c", c);
		else if (c == '\r')
			printf("\n");
		else if (c < 32)
		{
			printf("^%c", 64 + c);
			index++;
		}
	}
}


unsigned long cons_read(char *string, unsigned long length)
{
	char *caractere;
	int erreur = 0;
	index = 0;
	if (!length)
		return 0;
	while (users)
		wait_clock(current_clock() + 1);
	users = 1;
	while (index < min(256, length))
	{
		erreur = preceive(stdin, (int *)&caractere);
		assert(!erreur);
		if (*caractere == '\r')
			break;
		else if (*caractere == 127)
		{
			
			if (index > 0)
			{
				if (echo)
			    	printf("\b");
				string[--index] = 0;
			}
		}
		else
		{
			write_echo(*caractere);
			string[index] = *caractere;
			index++;
		}
	}
	printf("\n");
	users = 0;
	preset(stdin);
	return index;
}


void keyboard_data(char *str)
{
	psend(stdin, (int)str);
}

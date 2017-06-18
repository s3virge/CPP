// HexDumpOutput.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <stdio.h>
#include <ctype.h>

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 16
#endif

#define HEXDUMP_TEST

void hexdump(void *mem, unsigned int len)
{
	unsigned int i, j;

	for (i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
	{
		/* print offset */
		if (i % HEXDUMP_COLS == 0)
		{
			printf("0x%06x: ", i);
		}

		/* print hex data */
		if (i < len)
		{
			printf("%02x ", 0xFF & ((char*)mem)[i]);
		}
		else /* end of block, just aligning for ASCII dump */
		{
			printf("   ");
		}

		/* print ASCII dump */
		if (i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
		{
			for (j = i - (HEXDUMP_COLS - 1); j <= i; j++)
			{
				if (j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				}
				else if (isprint(((char*)mem)[j])) /* printable char */
				{
					putchar(0xFF & ((char*)mem)[j]);
				}
				else /* other char */
				{
					putchar('.');
				}
			}
			putchar('\n');
		}
	}
}

#ifdef HEXDUMP_TEST
int main(int argc, char *argv[])
{
	char test[] = {"Notice that the word one again disappears, letting you know that the macro again ran successfully."
		"You can cancel the running of a macro, such as when it is in an infinite loop, by clicking the spinning cassette icon in the Status Bar.You can also debug macros in the Macros IDE.For details about this, see Debugging Macros.For additional tips about running macros, see Macro Recording and Running Issues."};
		
		hexdump(test, sizeof(test));

	getchar();

	return 0;
}
#endif
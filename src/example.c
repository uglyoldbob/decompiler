#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stupid(int argc)
{
	int a = 42;
	
	if (a == 10)
	{
		printf("a = 10\n");
	}
	
	if (argc < 3)
	{
		a += 5;	
		printf("Not enough arguments\n");
	}
	else if (argc > 3)
	{
		a -= 5;
		printf("Too many arguments\n");
	}
	else
	{
		a += 10;
		printf("42\n");
	}

	if (a > 4)
	{
		printf("a > 4\n");
	}
	else if (a > 5)
	{
		printf("a > 5\n");
	}
	else if (a > 6)
	{
		printf("a > 6\n");
	}
	else
	{
		printf("nothing\n");
	}

	if (a < 4)
	{
		printf("a > 4\n");
	}
	else if (a < 5)
	{
		printf("a > 5\n");
	}
	else if (a < 6)
	{
		printf("a > 6\n");
	}


	
	if (a <= 9)
	{
		printf("peanuts\n");
	}
	else if (a >= 100)
	{
		printf("a >= 100\n");
	}

	if ((a >= 5) && (a <= 7))
	{
		printf("a is between 4 and 8\n");
	}
	else if ((a <= 15) || (a >= 20))
	{
		printf("a is not 16,17,18,19, or 20\n");
	}

	if ((a >= 5) && (a <= 7))
	{
		printf("a is between 4 and 8\n");
	}
	else if ((a <= 15) || (a >= 20))
	{
		printf("a is not 16,17,18,19, or 20\n");
	}
	else
	{
		printf("a is something else\n");
	}

	if ( (a <=5 ) && (a >= 1))
	{
		printf("yay food %d\n", a);
	}
	
	int i;
	for (i = 5; i < 100; i *= 2)
	{
		printf("i = %d\n", i);
	}

	int done = 0;
	while (!done)
	{
		if (a > 100)
			a = a / 10;
		if (a > 20)
			a += 81;
		if (a < 10)
			done = 1;
	}


	do
	{
		a *= 2;
		a -= 1;
	} while (a < 1024);

	if (a > 1)
	{
		printf("a > 1\n");
		if (a > 2)
		{
			printf("a > 2\n");
			if (a > 3)
			{
				printf("a > 3\n");
			}
			printf("a > 2\n");
		}
		printf("a > 1\n");
	}
}

int main(int argc, char *argv[])
{
	stupid(argc);
	return 0;

}

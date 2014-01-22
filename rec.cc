#include <stdio.h>

const char *n = "REC CALL";

const char *rec_fun()
{
	// -O2 not core
	// else core
	return rec_fun();
}


int main()
{

	printf("%s\n", rec_fun());
	return 0;
}

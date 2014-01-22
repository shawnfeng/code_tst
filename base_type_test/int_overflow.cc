#include <stdio.h>

int main()
{
	unsigned char v;
	for (int i = 0; i < 300; i++) {
		unsigned char b = v;
		unsigned char a = ++v;
		printf("%d %d %d %d\n", b % 10, b, a, b+1);
	}

	return 0;
}

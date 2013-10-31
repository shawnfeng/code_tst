#include <stdio.h>


class VirTest{
public:
	char a;
	char b;

};

class VirTest1{
public:
	char a;
	char b;

	virtual void f() {}

};


int main()
{
	VirTest v;
	VirTest1 v1;

	printf("v=%p v.a=%p v.b=%p\n", &v, &v.a, &v.b);
	printf("v1=%p v1.a=%p v1.b=%p\n", &v1, &v1.a, &v1.b);


	return 0;
}

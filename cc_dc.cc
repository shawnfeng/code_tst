#include <stdio.h>
class A {
public:
	void f()
	{
		printf("A.f\n");
	}

};

int main()
{
	A a;
	void (A::*p)() = &A::f;

	(a.*p)();
	((&a)->*p)();

	//void (*p2)(A *) = &A::f;



	return 0;
}

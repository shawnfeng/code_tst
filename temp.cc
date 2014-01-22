#include <stdio.h>

template <class T>
class Ct : public T
{
public:
	void fun()
	{
		printf("CHILD FUC\n");
		T::bfun();
	}
};

class Pt {
public:
	void bfun()
	{
		printf("FATHER FUC\n");
	}

};

int main()
{
	Ct<Pt> c;
	c.fun();
	return 0;
}

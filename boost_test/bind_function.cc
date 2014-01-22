#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <iostream>
using namespace std;

class Foo
{
public:
	void methodA()
	{
		cout << "call Foo::methodA" << endl;
	}
	void methodInt(int a)
	{
		cout << "call Foo::methodInt " << a << endl;
	}
};

class Bar
{
public:
	void methodB()
	{
		cout << "call Bar::methodB" << endl;
	}
};


int main()
{

	boost::function<void()> f1; // 无参数，无返回值

	Foo foo;
	f1 = boost::bind(&Foo::methodA, &foo);
	f1(); // 调用 foo.methodA();
	Bar bar;
	f1 = boost::bind(&Bar::methodB, &bar);
	f1(); // 调用 bar.methodB();

	f1 = boost::bind(&Foo::methodInt, &foo, 42);
	f1(); // 调用 foo.methodInt(42);

	boost::function<void(int)> f2; // int 参数，无返回值
	f2 = boost::bind(&Foo::methodInt, &foo, _1);
	f2(53); // 调用 foo.methodInt(53);

	boost::function<void(int)> f3;
	//f3(11); // core
	if (f3)
		f3(11); // OK

	boost::function<void(int)> f4 = NULL;
	f4.swap(f2);
	f4(33);


	boost::function<void(int)> f5; // int 参数，无返回值
	//f5 = boost::bind(&Foo::methodInt, &foo); // compile error


	return 1;
}

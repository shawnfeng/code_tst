#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <typeinfo>
#include <iostream>

using namespace std;
using namespace boost;


class SmartTest {
	const char *key_;
public:
	SmartTest(const char *k) : key_(k)
	{
		printf("%s CCCCC\n", key_);
	}

	~SmartTest()
	{
		printf("%s DDDDD\n", key_);
	}

	void print()
	{
		printf("%s PPPPPP\n", key_);
	}

};


int main()
{
	shared_ptr<SmartTest> s(new SmartTest("KEY_A"));
	printf("%ld\n", s.use_count());
	any a = s;
	printf("%ld\n", s.use_count());
	shared_ptr<SmartTest> &s1 = any_cast< shared_ptr<SmartTest> &>(a);
	printf("%ld\n", s.use_count());

	if (a.empty()) {
		printf("a NULL\n");
	}
	any b;
	if (b.empty()) {
		printf("b NULL\n");
	}

	//std::type_info dd = typeid(int);
	cout << typeid(int).name() << endl;
	cout << typeid(b).name() << endl;

	return 0;
}

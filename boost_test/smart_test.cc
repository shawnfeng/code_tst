#include <stdio.h>

#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace std;

class SmartTest {
public:
	SmartTest()
	{
		printf("CCCCC\n");
	}

	~SmartTest()
	{
		printf("DDDDD\n");
	}

};

class SmartTest2 {
	const char *key_;
public:
	SmartTest2(const char *k) : key_(k)
	{
		printf("%s CCCCC\n", key_);
	}

	~SmartTest2()
	{
		printf("%s DDDDD\n", key_);
	}

	void print()
	{
		printf("%s PPPPPP\n", key_);
	}

};


void test(const shared_ptr<SmartTest> &sp)
{
	printf("use count %ld\n", sp.use_count());
}

void test1(const shared_ptr<SmartTest> sp)
{
	printf("use count %ld\n", sp.use_count());
}


int main()
{
	SmartTest *t;
	{
		shared_ptr<SmartTest> sp(new SmartTest);
		printf("use count %ld\n", sp.use_count());
		t = sp.get();
	}
	puts("OUT");

	test(shared_ptr<SmartTest>(new SmartTest));
	test1(shared_ptr<SmartTest>(new SmartTest));

	puts("OVER");

	shared_ptr<SmartTest> sp0(new SmartTest);
	printf("use count %ld\n", sp0.use_count());
	test(sp0);
	test1(sp0);

	printf("test tongyong\n");
	shared_ptr<void> sv(new SmartTest2("tongyong"));
	printf("sv use count %ld\n", sv.use_count());

	const shared_ptr<SmartTest2> &ss = (const shared_ptr<SmartTest2> &)(sv);
	printf("sv use count %ld\n", sv.use_count());
	printf("ss use count %ld %p %p\n", ss.use_count(), &ss, &sv);
	ss->print();

	const shared_ptr<SmartTest2> ss2 = (shared_ptr<SmartTest2> &)(sv);
	//shared_ptr<SmartTest2> ss2 = (shared_ptr<SmartTest2>)(sv); // compile error
	printf("sv use count %ld\n", sv.use_count());
	printf("ss use count %ld\n", ss.use_count());
	printf("ss2 use count %ld\n", ss2.use_count());
	ss2->print();


	const shared_ptr<SmartTest2> &ss3 = boost::static_pointer_cast<SmartTest2, void>(sv);
	printf("ss3 use count %ld\n", ss3.use_count());
	ss3->print();


	const shared_ptr<void> &sv2 = (const shared_ptr<void> &)ss2;
	printf("sv2 use count %ld %p %p\n", sv2.use_count(), &sv2, &ss2);


	const shared_ptr<void> &sv1 = ss2;
	printf("sv1 use count %ld %p %p\n", sv1.use_count(), &sv1, &ss2);


	shared_ptr<void> sv3((void *)(new SmartTest2("内存泄漏了why?,应该不是内存泄漏，而是没有调用xigou函数")));
	printf("sv3 use count %ld\n", sv3.use_count());

	void *vp = new SmartTest2("内存泄漏原因");
	//  warning: deleting ‘void*’ is undefined [enabled by default]
	delete vp; // 内存释放，但是没有调用xigou函数
	SmartTest2 *vp2 = (SmartTest2 *)vp;
	//delete vp2; //core 重复回收内存

	return 0;
}

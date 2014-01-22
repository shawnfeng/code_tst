#include <stdio.h>

class Ct {
public:
	void print2()
	{
		puts("Ct");
	}
};


class Cdy {
public:
	virtual void just_vir() = 0;  // 必须要有，否则dynamic_cast编译不过去
};

class Cdy0 : public Cdy {
public:
	virtual void just_vir() {}
	void print()
	{
		puts("Cdy0");
	}

};

class Cdy1 : public Cdy {
public:
	virtual void just_vir() {}
	void print()
	{
		puts("Cdy1");
	}

};


class Ca {
public:
	template <class T>
	void print2(T &a)
	{
		a.print2();
	}

	template <class T>
	void print(Cdy *a)
	{
		T *t = (T *)a;
		t->print();
	}

	template <class T>
	void print3(Cdy &a)
	{
		T &t = (T &)a;
		t.print();
	}


	template <class T>
	void print4(Cdy *a)
	{
		T *t = dynamic_cast<T *>(a);
		printf("print4 %p\n", t);
		t->print();
	}

	template <class T>
	void print5(Cdy &a)
	{
		T &t = dynamic_cast<T &>(a);
		//printf("print4 %p\n", t);
		t.print();
	}


};


int main()
{
	Ct t;
	Ca a;
	Cdy0 d;
	Cdy1 d1;

	a.print2(t);
	//a.print(&d);
	a.print<Cdy0>(&d);
	a.print3<Cdy1>(d);
	a.print3<Cdy1>(d1);

	a.print4<Cdy1>(&d);
	a.print5<Cdy1>(d1);
	// core
	//a.print5<Cdy1>(d);

	Cdy &ttt = d;
	// core
	//Cdy1 &tt = dynamic_cast<Cdy1 &>(ttt);
	//tt.print();

	Cdy *ttt2 = &d;
	Cdy1 *tt2 = dynamic_cast<Cdy1 *>(ttt2);
	printf("%p\n", tt2);
	// 也可以成功
	tt2->print();


	return 0;
}

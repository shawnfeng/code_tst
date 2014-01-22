#include <string>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int main()
{

	string with_title = "dadasdf\n" "<span class=\"with-info\">" "" "</span>" "ddddddddddddd";

	with_title = "dadf";
	with_title = "&lt;span class=&quot;with-info&quot;&gt;和&lt;a target=&quot;_blank&quot; namecard=&quot;432702529&quot; href=&quot;http://www.renren.com/432702529&quot;&gt;孙一猫&lt;/a&gt;一起  &lt;/span&gt;111111111111111";
	string bwith_title = with_title;


	//const string pref = "<span class=\"with-info\">";
	//const string surf = "</span>";

	const string pref = "&lt;span class=&quot;with-info&quot;&gt;";
	const string surf = "&lt;/span&gt;";


	size_t b = with_title.find(pref);
	size_t e = with_title.rfind(surf);
	if (b != string::npos || e != string::npos) {
		with_title.replace(b, e-b+surf.size(), "");

	}


	cout << "b:" << b << " e:" << e << " bef:" << bwith_title << endl << "end:" << with_title << endl;


	cout << "map max min test"  << endl;

	multimap<double, int> sm;


	for (int i = 0; i < 10; ++i) {
		sm.insert(pair<double, int>(i+0.132001, i));
	}

	for (int i = 0; i < 10; ++i) {
		sm.insert(pair<double, int>((double)i+(double)0.132001, i));
	}

	for (int i = 0; i < 10; ++i) {
		sm.insert(pair<double, int>((double)i+(double)0.132001, i+1));
	}




	for (map<double, int>::const_iterator it = sm.begin(); it != sm.end(); ++it) {
		printf("%lf %d\n", it->first, it->second);
		//cout << it->first << " " << it->second << endl;
	}

	cout << endl;
	cout << sm.begin()->first << " " << sm.begin()->second << endl;
	cout << sm.rbegin()->first << " " << sm.rbegin()->second << endl;

	double ch = 2.132001;
	std::pair <std::multimap<double,int>::iterator, std::multimap<double,int>::iterator> ret;
	ret = sm.equal_range(ch);

	printf("ch=%lf => \n", ch);
	for (std::multimap<double,int>::iterator it=ret.first; it!=ret.second;) {
		printf("bef  %d\n", it->second);
		if (it->second == 2) {
			sm.erase(it++);
		} else {
			it++;
		}
		printf("aft  %d\n", it->second);

	}

	for (map<double, int>::const_iterator it = sm.begin(); it != sm.end(); ++it) {
		printf("%lf %d\n", it->first, it->second);
		//cout << it->first << " " << it->second << endl;
	}


	return 0;
}

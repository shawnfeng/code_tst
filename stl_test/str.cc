#include <string>
#include <iostream>

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


	return 0;
}

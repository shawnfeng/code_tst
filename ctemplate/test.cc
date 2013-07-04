#include <ctemplate/per_expand_data.h>
#include <ctemplate/template.h>
#include <ctemplate/template_dictionary.h>
#include <ctemplate/template_dictionary_interface.h>
#include <ctemplate/template_emitter.h>
#include <ctemplate/template_enums.h>
#include <ctemplate/template_modifiers.h>
#include <ctemplate/template_namelist.h>
#include <ctemplate/template_pathops.h>
#include <ctemplate/template_string.h>
#include <stdio.h>
#include <string>
#include <iostream>

// If you used ./configure --enable-namespace=foo, replace 'ctemplate'
// here with 'foo'.

using namespace std;

int main() {
	const char *t = "/home/shawn/ssh.do/ctemplate-2.2/src/tests/test.tpl";
	const char *ti = "/home/shawn/ssh.do/ctemplate-2.2/src/tests/test_i.tpl";

	ctemplate::Strip cst;

	int strip = 2;
	if (strip == 0) cst = ctemplate::DO_NOT_STRIP;
	else if (strip == 1) cst = ctemplate::STRIP_BLANK_LINES;
	else if (strip == 2) cst = ctemplate::STRIP_WHITESPACE;
	else cst = ctemplate::STRIP_WHITESPACE;


	//------------------------------                                                                                                     


	for (int i = 0; i < 100; ++i) {
		ctemplate::Template* tpl = ctemplate::Template::GetTemplate(t, cst);
		if (!tpl) {
			cout << "get template error!" << endl;
			return 1;

		}

		cout << i << "========================================" << endl;
		ctemplate::TemplateDictionary dict("test");

		ctemplate::TemplateDictionary *sdict = dict.AddIncludeDictionary("TEST_INCLUDE");
		sdict->SetFilename(ti);
		//sdict->SetValue("INC_T", "OOOOO");
		//dict.SetValue("INC_T", "OOOOO");
		dict.SetTemplateGlobalValue("INC_T", "OOOOO");
		//string username = "JMX";
		string username;
		dict.SetValue("GIRL", "MS");
		dict.SetValueAndShowSection("USERNAME", username, "CHANGE_USER");

		ctemplate::TemplateDictionary *copy_dict = dict.MakeCopy("copy_dict");
		dict.ShowSection("FUCK");
		//dict.RemoveSection("FUCK");
		//cout << "hidden check FUCK:" << dict.IsHiddenSection("FUCK")
		//     << "hidden check ADD_FUCK:" << dict.IsHiddenSection("ADD_FUCK") << endl;

		//cout << "hidden check FUCK:" << dict.GetValue("FUCK")
		//     << "hidden check ADD_FUCK:" << dict.GetValue("ADD_FUCK") << endl;

		//dict.GetValue("FUCK");
		//cout << "[FUCK]:" << dict["FUCK"];
		//dict["FUCK"] = 0;



		dict.AddSectionDictionary("ADD_FUCK");
		string rv;
		bool ok = tpl->Expand(&rv, &dict);
		if (ok) {
			cout << "res:" << rv << endl;
		} else {
			cout << "render error" << endl;
		}
		cout << "**************************************************" << endl;
		rv.clear();
		ok = tpl->Expand(&rv, copy_dict);
		if (ok) {
			cout << "res:" << rv << endl;
		} else {
			cout << "render error" << endl;
		}
		delete copy_dict;
		sleep(2);

		break;

		if (i % 10 == 0) {
			cout << "RELOAD----------" << endl;
			ctemplate::Template::ReloadAllIfChanged();
		}
	}
	return 0;
}

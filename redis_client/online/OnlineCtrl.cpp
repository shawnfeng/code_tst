#include "OnlineCtrl.h"

using namespace std;
/*
const char *tst = "eval \"local uid = KEYS[1]\n\
local session = KEYS[2]\n\
return uid..session\n\" 2 t0 t1";
*/
//const char *tst = "eval \"return 1\" 0";
//const char *tst = "eval \"return 'hello'\" 0";
//const char *tst = "SCRIPT LOAD \"return 'hello moto'\"";
const char *tst = "EVALSHA '232fd51614574cf0867b83d384a5e898cfd24e5a' 0";
//const char *tst = "eval \"return redis.call('GET', 'key0')\" 0";
//const char *tst = "eval \"return {KEYS[1],KEYS[2],ARGV[1],ARGV[2]}\" 2 key1 key2 first second";

//const char *tst = "GET key0";

static LogOut g_log;


void OnlineCtrl::online(long uid,
			const string &session,
			const map<string, string> &kvs)
{


	vector<string> hash;
	vector<std::string> rv;
	//rc_.cmd(rv, hash, 100, "EVAL  %s %d", "return 'hello'", 0);
	rc_.cmd(rv, hash, 100, "EVAL  %s %d %s", "return redis.call('get', KEYS[1])", 1, "key0");
	//rc_.cmd(rv, hash, 100, "EVAL %s %d", "232fd51614574cf0867b83d384a5e898cfd24e5a", 0);
	//rc_.cmd(rv, hash, 100, "GET key0");
	//rc_.cmd(rv, hash, 100, "GET %b", "key0", 4);
	//rc_.cmd(rv, hash, 100, "SET %s %s", "foo", "hello world");

	for (vector<string>::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		g_log.debug("=== %s ===", it->c_str());
	}


}



int main (int argc, char **argv)
{
	g_log.debug("%u", sizeof(int));
	g_log.debug("%u", sizeof(long));
	g_log.debug("%u", sizeof(long long));
	g_log.debug("%u", sizeof(long long int));


	OnlineCtrl oc;

	long uid = 0;
	string session;
	map<string, string> kvs;
	

	sleep(1);
	oc.online(uid, session, kvs);
	g_log.debug("=================");
	sleep(1);
	oc.online(uid, session, kvs);

	pause();
	

	return 0;
}


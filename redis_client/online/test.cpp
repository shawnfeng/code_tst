#include "OnlineCtrl.h"
using namespace std;

int main (int argc, char **argv)
{
	LogOut g_log;
	g_log.debug("%u", sizeof(int));
	g_log.debug("%u", sizeof(long));
	g_log.debug("%u", sizeof(long long));
	g_log.debug("%u", sizeof(long long int));


	OnlineCtrl oc(LogOut::log_trace, LogOut::log_debug, LogOut::log_info, LogOut::log_warn, LogOut::log_error,
                "/data/home/guangxiang.feng/code_tst/redis_client/script");

	long uid = 1000;
	string session = "fuck2";
	vector<string> kvs;
	kvs.push_back("k3");
	kvs.push_back("v3");


	sleep(1);
	oc.online(uid, session, kvs);
	g_log.debug("=================");
	sleep(20);
	oc.online(uid, session, kvs);
	//oc.offline(uid, session);

	vector<string> ks;
  oc.get_sessions(uid, ks);

  map<string, string> svs;
  oc.get_session_info(uid, session, ks, svs);

	pause();
	

	return 0;
}


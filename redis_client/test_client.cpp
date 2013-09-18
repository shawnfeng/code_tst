#include <sys/syscall.h> 

#include "RedisClient.h"

using namespace std;
static void log_trace(const char *log)
{
	
	printf("[T]:[%ld]:%s\n", syscall(SYS_gettid), log);
}


static void log_debug(const char *log)
{
	printf("[D]:[%ld]:%s\n", syscall(SYS_gettid), log);
}

static void log_info(const char *log)
{
	printf("[I]:[%ld]:%s\n", syscall(SYS_gettid), log);
}

static void log_warn(const char *log)
{
	printf("[W]:[%ld]:%s\n", syscall(SYS_gettid), log);
}

static void log_error(const char *log)
{
	printf("[E]:[%ld]:%s\n", syscall(SYS_gettid), log);
}


static LogOut g_log(log_trace, log_debug, log_info, log_warn, log_error);



int main (int argc, char **argv)
{

	g_log.info("MAIN-->RediClient init");
	RedisClient rc(log_trace, log_debug, log_info, log_warn, log_error);

	rc.start();
	g_log.info("MAIN-->RedisClient start");
	g_log.info("sleep ZZZ");
	//sleep(1);

	vector< pair<string, int> >addrs;
	addrs.push_back(pair<string, int>("127.0.0.1", 10010));
	addrs.push_back(pair<string, int>("127.0.0.1", 10020));
	addrs.push_back(pair<string, int>("10.2.72.23", 10010));
	rc.update_ends(addrs);

	//sleep(1);
	//g_log.info("MAIN-->call disconnect");
	//redisAsyncDisconnect(rc.test());


	//	sleep(1000);

	sleep(10);
	vector<string> hash;
	hash.push_back("127.0.0.1:10010");
	hash.push_back("127.0.0.1:10020");
	hash.push_back("10.2.72.23:10010");
	for (int i = 0; i < 100; ++i) {
		g_log.info("%d sleep ZZZZZZ", i);
		sleep(1);


		rc.cmd(hash, "GET key0", 100);
	}



	g_log.info("MAIN-->hold here");
	pause();
	
	return 1;

}

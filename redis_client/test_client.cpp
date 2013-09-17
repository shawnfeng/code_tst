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

	vector< pair<string, int> >ip_port;
	rc.update_ends(ip_port);


	rc.start();
	g_log.info("MAIN-->RedisClient start");

	g_log.info("sleep ZZZ");
	sleep(3);

	vector<string> hash;
	rc.cmd(hash, "GET key0", 10);

	g_log.info("MAIN-->hold here");
	pause();
	
	return 1;

}

#include <sys/syscall.h> 
#include <arpa/inet.h>
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






void *thread_cb(void* args)
{  
	RedisClient *rc = (RedisClient *)args;
	vector<string> hash;

	for (int i = 0; i < 2000; ++i) {
		rc->cmd(hash, "GET key0", 100);
	}

	return NULL;

}


int main (int argc, char **argv)
{

	//long ip = inet_addr("10.3.2.3");
	//g_log.info("ip=%ld", ip);
	g_log.info("ulong=%lu", sizeof(ulong));
	g_log.info("long=%lu", sizeof(long));
	g_log.info("int=%lu", sizeof(int));
	g_log.info("uint64_t=%lu", sizeof(uint64_t));
	g_log.info("uint32_t=%lu", sizeof(uint32_t));
	g_log.info("in_addr_t=%lu", sizeof(in_addr_t));

	uint64_t ipv4 = ipv4_int64("10.3.2.3", 333);
	g_log.info("ip=%ld", ipv4);

	char buff[100];
	int port;

	int64_ipv4(ipv4, buff, 100, port);
	g_log.info("ip=%s,port=%d", buff, port);


	//=================================
	g_log.info("MAIN-->RediClient init");
	RedisClient rc(log_trace, log_debug, log_info, log_warn, log_error);

	rc.start();

	vector< pair<string, int> > ends;
	ends.push_back(pair<string, int>("127.0.0.1", 10010));
	ends.push_back(pair<string, int>("127.0.0.1", 10020));
	ends.push_back(pair<string, int>("10.2.72.23", 10010));

	rc.update_ends(ends);
	g_log.info("MAIN-->RedisClient start");
	g_log.info("sleep ZZZ");
	// ============================

	pthread_t pids[20];
	int pn = (int)(sizeof(pids)/sizeof(pids[0]));
	for (int i = 0; i < pn; ++i) {
		if (pthread_create(&pids[i], NULL, thread_cb, (void *)&rc)) {
			printf("create thread error!\n");
			return -1;  
		}

	}

	for (int i = 0; i < pn; ++i) {
		pthread_join(pids[i],NULL);
	}


	g_log.info("MAIN-->hold here");
	//	pause();
	
	return 1;

}

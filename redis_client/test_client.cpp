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

static void connectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        g_log.error("error: %s", c->errstr);
        return;
    }

    g_log.info("Connected...");
}

static void disconnectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        g_log.error("Error: %s", c->errstr);
        return;
    }

    g_log.info("Disconnected...");
}


int main (int argc, char **argv)
{

	g_log.info("MAIN-->RedisEvent init");
	RedisEvent rc(log_trace, log_debug, log_info, log_warn, log_error);

	rc.start();
	g_log.info("MAIN-->RedisEvent start");

	redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 10010);
	redisAsyncContext *c1 = redisAsyncConnect("127.0.0.1", 10020);
	redisAsyncContext *c2 = redisAsyncConnect("10.2.72.23", 10010);

	if (c->err) {
		g_log.error("Error: %s", c->errstr);
		return 1;
	}


	if (c1->err) {
		g_log.error("Error1: %s", c->errstr);
		return 1;
	}

	if (c2->err) {
		g_log.error("Error1: %s", c->errstr);
		return 1;
	}


	redisAsyncSetConnectCallback(c, connectCallback);
	redisAsyncSetDisconnectCallback(c, disconnectCallback);

	g_log.info("sleep ZZZ");


	rc.attach(c1);
	rc.attach(c);
	sleep(3);
	vector<redisAsyncContext *> rcs;
	rcs.push_back(c);
	rcs.push_back(c1);
	rc.cmd(rcs, "GET key0", 10);

	g_log.info("MAIN-->hold here");
	pause();
	
	return 1;

}

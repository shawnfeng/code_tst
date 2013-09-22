#include <sys/syscall.h>
#include <arpa/inet.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>


#include "Util.h"


uint64_t ipv4_int64(const char *ip, int port)
{
	uint64_t ipv4 = 0;
	uint64_t uip = inet_addr(ip);
	ipv4 = (uip << 32) | (uint32_t)port;

	return ipv4;
	
}

void int64_ipv4(uint64_t ipv4, char *ip, size_t len, int &port)
{
	uint64_t uip = ipv4 >> 32;
	port = ipv4 & 0x00000000FFFFFFFF;

	inet_ntop(AF_INET, &uip, ip, len);
	
}

void LogOut::log_trace(const char *log)
{
	
	printf("[T]:[%ld]:%s\n", syscall(SYS_gettid), log);
}


void LogOut::log_debug(const char *log)
{
	printf("[D]:[%ld]:%s\n", syscall(SYS_gettid), log);
}

void LogOut::log_info(const char *log)
{
	printf("[I]:[%ld]:%s\n", syscall(SYS_gettid), log);
}

void LogOut::log_warn(const char *log)
{
	printf("[W]:[%ld]:%s\n", syscall(SYS_gettid), log);
}

void LogOut::log_error(const char *log)
{
	printf("[E]:[%ld]:%s\n", syscall(SYS_gettid), log);
}


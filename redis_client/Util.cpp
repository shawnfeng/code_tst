#include <arpa/inet.h>

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

#include <arpa/inet.h>

#include "Util.h"

ulong ipv4_ulong(const char *ip, uint port)
{
	ulong ipv4 = 0;
	in_addr_t uip = inet_addr(ip);
	ipv4 = (uip << 32) | port;

	return ipv4;
	
}

#include "RedisClient.h"

using namespace std;

void RedisClient::cmd(std::vector<std::string> &rv, const std::vector<std::string> &hash, int timeout, const char *format, ...)
{
	set<uint64_t> addrs;
	rcx_.hash_addr(hash, addrs);

	va_list args;
	va_start(args, format);
	re_.cmd(rv, addrs, timeout, format, args);
	va_end(args);
}

void RedisClient::cmd(const std::vector<long> &hash, const char *c, int timeout, std::vector<std::string> &rv)
{

}


#include "RedisClient.h"

using namespace std;

void RedisClient::cmd(const std::vector<std::string> &hash, const char *c, int timeout)
{
	set<uint64_t> addrs;
	rcx_.hash_rcx(hash, addrs);
	re_.cmd(addrs, c, timeout);
}

void RedisClient::cmd(const std::vector<long> &hash, const char *c, int timeout)
{

}


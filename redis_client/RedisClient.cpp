#include "RedisClient.h"

using namespace std;

void RedisClient::cmd(const std::vector<std::string> &hash, const char *c, int timeout, std::vector<std::string> &rv)
{
	set<uint64_t> addrs;
	rcx_.hash_addr(hash, addrs);
	re_.cmd(addrs, c, timeout, rv);
}

void RedisClient::cmd(const std::vector<long> &hash, const char *c, int timeout, std::vector<std::string> &rv)
{

}


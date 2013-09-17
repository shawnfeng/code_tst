#include "RedisClient.h"

using namespace std;

void RedisClient::cmd(const std::vector<std::string> &hash, const char *c, int timeout)
{
	set<redisAsyncContext *> rcxs;
	rcx_.hash_rcx(hash, rcxs);
	re_.cmd(rcxs, c, timeout);
}

void RedisClient::cmd(const std::vector<long> &hash, const char *c, int timeout)
{

}


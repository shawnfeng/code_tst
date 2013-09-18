#include "RedisHash.h"
using namespace std;

void RedisHash::update_ends(const std::vector< std::pair<std::string, int> > &ends)
{
	const char *fun = "RedisHash::update_ends";
	log_->info("%s-->size:%lu", fun, ends.size());
}

void RedisHash::hash_addr(const std::vector<std::string> &hash, std::set<uint64_t> &addrs)
{
	const char *fun = "RedisHash::hash_addr";


	uint64_t a0 = ipv4_int64("127.0.0.1", 10010);
	uint64_t a1 = ipv4_int64("127.0.0.1", 10020);
	uint64_t a2 = ipv4_int64("10.2.72.23", 10010);

	addrs.insert(a0);
	addrs.insert(a1);
	addrs.insert(a2);


	log_->debug("%s-->hash:%lu addrs:%lu", fun, hash.size(), addrs.size());
}



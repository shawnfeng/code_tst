#include "RedisHash.h"
using namespace std;

void RedisHash::update_ends(const vector< pair<string, int> > &ends)
{
	const char *fun = "RedisHash::update_ends";
	log_->info("%s-->size:%lu", fun, ends.size());

	set<uint64_t> addrs;
	for (vector< pair<string, int> >::const_iterator it = ends.begin();
		     it != ends.end();
		     ++it) {
		uint64_t addr = ipv4_int64(it->first.c_str(), it->second);
		addrs.insert(addr);
	}

	boost::unique_lock< boost::shared_mutex > lock(smux_);
	addrs_.swap(addrs);
}

void RedisHash::hash_addr(const std::vector<std::string> &hash, std::set<uint64_t> &addrs)
{
	const char *fun = "RedisHash::hash_addr";


	boost::shared_lock< boost::shared_mutex > lock(smux_);
	// just test give all
	addrs = addrs_;

	log_->debug("%s-->hash:%lu addrs:%lu", fun, hash.size(), addrs.size());
}



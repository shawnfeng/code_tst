#ifndef __REDIS_HASH_H_H__
#define __REDIS_HASH_H_H__

#include <vector>
#include <map>
#include <string>

#include <boost/thread/mutex.hpp>


#include "Util.h"
#include "RedisEvent.h"


class RedisHash {
	LogOut *log_;
	RedisEvent *re_;
	boost::shared_mutex smux_;
	std::set<uint64_t> addrs_;

 public:

 RedisHash(LogOut *log, RedisEvent *re) : log_(log), re_(re) {
	}
	void update_ends(const std::vector< std::pair<std::string, int> > &ends);

	void hash_addr(const std::vector<std::string> &hash, std::set<uint64_t> &addrs);
	LogOut *log() { return log_; }


};

#endif

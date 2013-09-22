#ifndef __REDIS_HASH_H_H__
#define __REDIS_HASH_H_H__

#include <vector>
#include <map>
#include <string>

#include <boost/thread/mutex.hpp>

#include <zookeeper/zookeeper.h>

#include "Util.h"
#include "RedisEvent.h"


class RedisHash {
	LogOut *log_;
	RedisEvent *re_;
	boost::shared_mutex smux_;
	std::set<uint64_t> addrs_;

	// zookeeper
	zhandle_t *zkh_;
	std::string zk_addr_;
	std::string zk_path_;

 public:

 RedisHash(LogOut *log, RedisEvent *re,
	   const char *zk_addr,
	   const char *zk_path)
	 : log_(log), re_(re),
		zkh_(NULL),
		zk_addr_(zk_addr),
		zk_path_(zk_path)
		{

		}
	void update_ends(const std::vector< std::pair<std::string, int> > &ends);

	int start();

	void hash_addr(const std::vector<std::string> &hash, std::set<uint64_t> &addrs);
	LogOut *log() { return log_; }


};

#endif

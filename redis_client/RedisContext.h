#ifndef __REDIS_CONTEXT_H_H__
#define __REDIS_CONTEXT_H_H__

#include <vector>
#include <map>
#include <string>

#include <boost/thread/mutex.hpp>

#include <adapters/libev.h>



class RedisContext {
	struct rds_c_t {
		enum { NONE, SYN, ESTABLISHED, CLOSED, };
		int st;
		redisAsyncContext *c;

	rds_c_t() : st(NONE), c(NULL) {}
	};

	boost::mutex mutex_;
	std::map<std::string, rds_c_t> contexts_;

 public:
	// RedisContext() : rc_(rc) {}
	void update_ends(std::vector<std::string> &ends);

};

#endif

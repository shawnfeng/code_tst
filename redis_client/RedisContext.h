#ifndef __REDIS_CONTEXT_H_H__
#define __REDIS_CONTEXT_H_H__

#include <vector>
#include <map>
#include <string>

#include <boost/thread/mutex.hpp>

#include <adapters/libev.h>

#include "Util.h"



class RedisContext {
	struct rds_c_t {
		enum { NONE, SYN, ESTABLISHED, CLOSED, };
		int st;
		redisAsyncContext *c;

	rds_c_t() : st(NONE), c(NULL) {}
	};
	LogOut *log_;
	struct ev_loop *loop_;
	boost::mutex mutex_;
	std::map<std::string, rds_c_t> ctxs_;

 public:

 RedisContext(LogOut *log) : log_(log), loop_(EV_DEFAULT) {
		log->info("%s-->loop:%p", "RedisContext::RedisContext", loop_);
	}
	void update_ends(std::vector< std::pair<std::string, int> > &ends);
	void attach(redisAsyncContext *c);

	void hash_rcx(const std::vector<std::string> &hash, std::vector<redisAsyncContext *> &rcxs);
	LogOut *log() { return log_; }


};

#endif

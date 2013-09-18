#ifndef __REDIS_CONTEXT_H_H__
#define __REDIS_CONTEXT_H_H__

#include <vector>
#include <map>
#include <string>

#include <boost/thread/mutex.hpp>

#include <adapters/libev.h>

#include "Util.h"
#include "RedisEvent.h"


class RedisContext {
	LogOut *log_;
	RedisEvent *re_;
	struct ev_loop *loop_;

	boost::shared_mutex smux_;
	std::map<std::string, redisAsyncContext *> ctxs_;

	redisAsyncContext *attach_ctx(const std::string &addr);
 public:

 RedisContext(LogOut *log, RedisEvent *re) : log_(log), re_(re), loop_(EV_DEFAULT) {
		log->info("%s-->loop:%p", "RedisContext::RedisContext", loop_);
	}
	void update_ends(const std::vector< std::pair<std::string, int> > &ends);
	//void attach(redisAsyncContext *c);

	void clear_ctx(const char *addr);

	void hash_rcx(const std::vector<std::string> &hash, std::set<redisAsyncContext *> &rcxs);
	LogOut *log() { return log_; }


	//redisAsyncContext *test;


};

#endif

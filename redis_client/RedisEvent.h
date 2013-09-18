#ifndef __REDIS_EVENT_H_H__
#define __REDIS_EVENT_H_H__
#include <vector>
#include <set>
#include <map>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <adapters/libev.h>

#include "Util.h"

class RedisEvent;

class cflag_t {
	int f_;
	void *d_;
public:
	cflag_t() : f_(0), d_(NULL) {}
	void reset() { f_ = 0; d_ = NULL; }

	int f() { return f_; }
	int d_f() { return --f_; }
	void *d() { return d_; }

	void set_f(int f) { f_ = f; }
	void set_d(void *d) { d_ = d; }
};


class userdata_t {

	enum { CUR_CALL_NUM = 100000, };
	size_t idx_;
	cflag_t cfg_[CUR_CALL_NUM];
	std::map<ulong, redisAsyncContext *> ctxs_;
	RedisEvent *re_;

 private:


public:
 userdata_t(RedisEvent *re) : re_(re)
		{
		}

	RedisEvent *re() { return re_; }

	cflag_t *get_cf()
	{ 
		if (++idx_ >= CUR_CALL_NUM) idx_ = 0;

		cflag_t *cf = &cfg_[idx_];
		return cf;
	}

	redisAsyncContext *lookup(ulong addr)
	{
		std::map<ulong, redisAsyncContext *>::const_iterator it = ctxs_.find(addr);
		if (it == ctxs_.end()) {
			ctxs_[addr] = NULL;
			return NULL;
		} else {
			return it->second;
		}
	}

};



class RedisEvent {
 private:
	LogOut *log_;
	struct ev_loop *loop_;

	ev_async async_w_;

	boost::mutex mutex_;
	userdata_t ud_;


 private:
	void run();

 public:
	RedisEvent(LogOut *log
		   ) : log_(log), loop_(EV_DEFAULT), ud_(this)
		{
			log->info("%s-->loop:%p", "RedisEvent::RedisEvent", loop_);
		}

	void lock() { mutex_.lock(); }
	void unlock() { mutex_.unlock(); }

	void attach(redisAsyncContext *c, const char *addr, void *data, redisConnectCallback *oncall, redisDisconnectCallback *discall);

	void connect(ulong addr);

	void start ();
	LogOut *log() { return log_; }

	void cmd(std::set<redisAsyncContext *> &rcxs, const char *c, int timeout);
	void cmd(std::set<ulong> &addrs, const char *c, int timeout);
	struct ev_loop *loop() { return loop_; }

};




#endif
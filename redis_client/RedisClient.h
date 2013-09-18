#ifndef __REDIS_CLIENT_H_H__
#define __REDIS_CLIENT_H_H__

#include "RedisEvent.h"
#include "RedisHash.h"

class RedisClient {
	LogOut log_;
	RedisEvent re_;
	RedisHash rcx_;
 public:
	// WARNING: the trace set is just use for trace the routine
	// process, which will cause low performance!
	// so NULL will be setted log_e for online environment
	RedisClient(void (*log_t)(const char *),
		    void (*log_d)(const char *),
		    void (*log_i)(const char *),
		    void (*log_w)(const char *),
		    void (*log_e)(const char *)
		    ) : log_(log_t, log_d, log_i, log_w, log_e),
		re_(&log_), rcx_(&log_, &re_)
		{}

	void start() { re_.start(); }
	void update_ends(std::vector< std::pair<std::string, int> > &ends) { rcx_.update_ends(ends); }
	void cmd(const std::vector<std::string> &hash, const char *c, int timeout);
	void cmd(const std::vector<long> &hash, const char *c, int timeout);

};

#endif

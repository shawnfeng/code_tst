#ifndef __REDIS_EVENT_H_H__
#define __REDIS_EVENT_H_H__
#include <vector>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>


#include <adapters/libev.h>
class RedisEvent;

class LogOut {
	void (*log_t_)(const char *);
	void (*log_d_)(const char *);
	void (*log_i_)(const char *);
	void (*log_w_)(const char *);
	void (*log_e_)(const char *);

	enum { BUFFER_SIZE = 1024, };

#define LOGOUT_FMT_LOG(format, log_f)				\
	if (log_f) {						\
		char buff[BUFFER_SIZE];				\
		va_list args;					\
		va_start(args, format);				\
		vsnprintf(buff, BUFFER_SIZE, format, args);	\
		va_end(args);					\
		log_f(buff);					\
	}

 public:
	LogOut(
	       void (*log_t)(const char *),
	       void (*log_d)(const char *),
	       void (*log_i)(const char *),
	       void (*log_w)(const char *),
	       void (*log_e)(const char *)
	       ) : log_t_(log_t), log_d_(log_d), log_i_(log_i), log_w_(log_w), log_e_(log_e)
	{}

	void trace(const char *format, ...) { LOGOUT_FMT_LOG(format, log_t_) }
	void debug(const char *format, ...) { LOGOUT_FMT_LOG(format, log_d_) }
	void info(const char *format, ...) { LOGOUT_FMT_LOG(format, log_i_) }
	void warn(const char *format, ...) { LOGOUT_FMT_LOG(format, log_w_) }
	void error(const char *format, ...) { LOGOUT_FMT_LOG(format, log_e_) }

};

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
	RedisEvent *rc_;

 private:


public:
 userdata_t(RedisEvent *rc) : rc_(rc)
		{
		}

	RedisEvent *rc() { return rc_; }

	cflag_t *get_cf()
	{ 
		if (++idx_ >= CUR_CALL_NUM) idx_ = 0;

		cflag_t *cf = &cfg_[idx_];
		return cf;
	}

};



class RedisEvent {
 private:
	struct ev_loop *loop_;

	ev_async async_w_;

	LogOut log_;
	boost::mutex mutex_;
	userdata_t ud_;


 private:
	void run();

 public:
	RedisEvent(void (*log_t)(const char *),
		    void (*log_d)(const char *),
		    void (*log_i)(const char *),
		    void (*log_w)(const char *),
		    void (*log_e)(const char *)
		    ) : log_(log_t, log_d, log_i, log_w, log_e), ud_(this)
		{}

	void lock() { mutex_.lock(); }
	void unlock() { mutex_.unlock(); }


	void start ();
	void attach(redisAsyncContext *c);
	void cmd(std::vector<redisAsyncContext *>rcs, const char *c, int timeout);


};




#endif

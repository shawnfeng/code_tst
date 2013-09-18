#ifndef __REDIS_UTIL_H_H__
#define __REDIS_UTIL_H_H__
#include <stdarg.h>
#include <stdio.h>



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

ulong ipv4_ulong(const char *ip, uint port);


#endif

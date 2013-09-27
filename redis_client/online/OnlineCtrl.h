#ifndef __ONLINE_CTRL_H_H__
#define __ONLINE_CTRL_H_H__

#include <string>

#include <RedisClient.h>

class OnlineCtrl {
	LogOut log_;
	RedisClient rc_;


 public:
 OnlineCtrl() : log_(LogOut::log_trace, LogOut::log_debug, LogOut::log_info, LogOut::log_warn, LogOut::log_error),
		rc_(LogOut::log_trace, LogOut::log_debug, LogOut::log_info, LogOut::log_warn, LogOut::log_error,
		    "127.0.0.1:4180,127.0.0.1:4181,127.0.0.1:4182",
		    "/tx/online/legal_nodes"
		    ) { rc_.start(); }


	void online(long uid, const std::string &session, const std::vector<std::string> &kvs);

};


#endif

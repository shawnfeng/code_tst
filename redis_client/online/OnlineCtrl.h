#ifndef __ONLINE_CTRL_H_H__
#define __ONLINE_CTRL_H_H__

#include <string>

#include "../src/RedisClient.h"

class OnlineCtrl {
	LogOut log_;
  std::string sp_;
	RedisClient rc_;


 public:
 OnlineCtrl(void (*log_t)(const char *),
            void (*log_d)(const char *),
            void (*log_i)(const char *),
            void (*log_w)(const char *),
            void (*log_e)(const char *),

            const char *path
            ) : log_(log_t, log_d, log_i, log_w, log_e),
    sp_(path),
		rc_(log_t, log_d, log_i, log_w, log_e,
		    "10.2.72.57:4180,10.2.72.57:4181,10.2.72.57:4182",
		    "/tx/online/legal_nodes"
		    ) { rc_.start(); }


	void online(long uid, const std::string &session, const std::vector<std::string> &kvs);
	void offline(long uid, const std::string &session);
	void get_sessions(long uid, std::vector<std::string> &sessions);
	void get_session_info(long uid, const std::string &session, const std::vector<std::string> &ks,
                        std::map<std::string, std::string> &kvs);

};


#endif

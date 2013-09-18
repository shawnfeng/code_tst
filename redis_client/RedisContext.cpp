#include "RedisContext.h"
using namespace std;
static void connectCallback(const redisAsyncContext *c, int status) {
	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	RedisContext *rtx = (RedisContext *)re->data;

	//rcx->log()->info("connectCallback c:%p", c);

	if (status != REDIS_OK) {
		rtx->log()->error("connectCallback c:%p %s", c, c->errstr);
		rtx->clear_ctx(re->addr);
		return;
	} else {
		re->status = 1;
		rtx->log()->info("connectCallback c:%p ok", c);
	}

}

static void disconnectCallback(const redisAsyncContext *c, int status) {

	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	RedisContext *rtx = (RedisContext *)re->data;

	//rcx->log()->info("disconnectCallback c:%p", c);
	if (status != REDIS_OK) {
		rtx->log()->error("disconnectCallback c:%p %s", c, c->errstr);
		return;
	} else {
		rtx->log()->info("disconnectCallback c:%p ok", c);
	}
	rtx->clear_ctx(re->addr);

}


void RedisContext::clear_ctx(const char *addr)
{
	const char *fun = "RedisContext::clear_ctx";
	log_->info("%s-->addr clear %s", fun, addr);

	boost::unique_lock< boost::shared_mutex > lock(smux_);

	map<string, redisAsyncContext *>::iterator it = ctxs_.find(addr);
	if (it != ctxs_.end()) {
		it->second = NULL;
	} else {
		log_->warn("%s-->addr can not find", fun, addr);
	}
}

void RedisContext::update_ends(const std::vector< std::pair<std::string, int> > &ends)
{
	const char *fun = "RedisContext::update_ends";
	log_->info("%s-->size:%lu", fun, ends.size());
	{
		char buff[200];
		boost::unique_lock< boost::shared_mutex > lock(smux_);
		log_->info("%s-->lock", fun);
		for (vector< pair<string, int> >::const_iterator it = ends.begin(); it != ends.end(); ++it) {
			snprintf(buff, sizeof(buff), "%s:%d", it->first.c_str(), it->second);
			log_->info("%s-->addr:%s", fun, buff);

			map<string, redisAsyncContext *>::iterator jt = ctxs_.find(buff);
			if (jt != ctxs_.end()) {
				continue;
			} else {
				ctxs_[buff] = attach_ctx(buff);
			}

			log_->info("!!!!!!!!%s-->addr:%s", fun, buff);

		}
		log_->info("!!!!!!!!!!!!!!%s-->unlock", fun);

	}
	log_->info("%s-->over", fun);
}

redisAsyncContext * RedisContext::attach_ctx(const string &addr)
{
	const char *fun = "RedisContext::attach_ctx";

	size_t pos = addr.find(":");
	if (pos == string::npos) {
		log_->error("%s-->error format 0 addr:%s", fun, addr.c_str());
		return NULL;
	}

	string ip = addr.substr(0, pos);
	int port = atoi(addr.substr(pos+1).c_str());

	if (port == 0 || ip.empty()) {
		log_->error("%s-->error format addr:%s ip:%s port:%d", fun, addr.c_str(), ip.c_str(), port);
		return NULL;
	} else {
		log_->info("%s-->format addr:%s ip:%s port:%d", fun, addr.c_str(), ip.c_str(), port);
		redisAsyncContext *c = redisAsyncConnect(ip.c_str(), port);
		c->ev.data = NULL;
		re_->attach(c, addr.c_str(), (void *)this, connectCallback, disconnectCallback);
		return c;
	}
}

void RedisContext::hash_rcx(const std::vector<std::string> &hash, std::set<redisAsyncContext *> &rcxs)
{

	const char *fun = "RedisContext::hash_rcx";

	vector<string> addrs = hash;

	vector<string> addrs_not_init;

	{
		boost::shared_lock< boost::shared_mutex > lock(smux_);
		for (vector<string>::const_iterator it = addrs.begin();
		     it != addrs.end(); ++it) {
			map<string, redisAsyncContext *>::iterator jt = ctxs_.find(*it);
			if (jt == ctxs_.end()) continue;

			if (jt->second) {
				rcxs.insert(jt->second);
			} else {
				addrs_not_init.push_back(*it);
			}
		}
	}

	if (!addrs_not_init.empty()) {
		log_->warn("%s-->addrs_not_init size:%lu", fun, addrs_not_init.size());
		boost::unique_lock< boost::shared_mutex > lock(smux_);
		for (vector<string>::const_iterator it = addrs_not_init.begin();
		     it != addrs_not_init.end(); ++it) {
			log_->info("%s-->addrs:%s", fun, it->c_str());
			map<string, redisAsyncContext *>::iterator jt = ctxs_.find(*it);
			if (jt == ctxs_.end()) continue;

			if (jt->second == NULL) {
				ctxs_[*it] = attach_ctx(*it);
			}

		}

	}
	

}



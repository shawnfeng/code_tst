#include "RedisContext.h"
using namespace std;
static void connectCallback(const redisAsyncContext *c, int status) {
	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	RedisContext *rcx = (RedisContext *)re->data;

	//rcx->log()->info("connectCallback c:%p", c);

	if (status != REDIS_OK) {
		rcx->log()->error("connectCallback c:%p %s", c, c->errstr);
		return;
	} else {
		rcx->log()->info("connectCallback c:%p ok", c);
	}

}

static void disconnectCallback(const redisAsyncContext *c, int status) {

	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	RedisContext *rcx = (RedisContext *)re->data;

	//rcx->log()->info("disconnectCallback c:%p", c);
	if (status != REDIS_OK) {
		rcx->log()->error("disconnectCallback c:%p %s", c, c->errstr);
		return;
	} else {
		rcx->log()->info("disconnectCallback c:%p ok", c);
	}

}



void RedisContext::update_ends(std::vector< std::pair<std::string, int> > &ends)
{
	const char *fun = "RedisContext::update_ends";
	redisAsyncContext *c0 = redisAsyncConnect("127.0.0.1", 10010);
	redisAsyncContext *c1 = redisAsyncConnect("127.0.0.1", 10020);
	redisAsyncContext *c2 = redisAsyncConnect("10.2.72.23", 10010);

	log_->info("c0: %p", c0);
	log_->info("c1: %p", c1);
	log_->info("c2: %p", c2);

	if (c0->err) {
		log_->error("Error: %s", c0->errstr);
		return;
	}

	if (c1->err) {
		log_->error("Error1: %s", c1->errstr);
		return;
	}

	if (c2->err) {
		log_->error("Error2: %s", c2->errstr);
		return;
	}

	attach(c0);
	attach(c1);
	attach(c2);


	rds_c_t r0, r1, r2;
	r0.c = c0; r0.st = rds_c_t::SYN;
	r1.c = c1; r1.st = rds_c_t::SYN;
	r2.c = c2; r2.st = rds_c_t::SYN;
	

	log_->info("%s-->conn cb %p", fun, c0);
	redisAsyncSetConnectCallback(c0, connectCallback);
	redisAsyncSetDisconnectCallback(c0, disconnectCallback);

	log_->info("%s-->conn cb %p", fun, c1);
	redisAsyncSetConnectCallback(c1, connectCallback);
	redisAsyncSetDisconnectCallback(c1, disconnectCallback);

	log_->info("%s-->conn cb %p", fun, c2);
	redisAsyncSetConnectCallback(c2, connectCallback);
	redisAsyncSetDisconnectCallback(c2, disconnectCallback);


	char buff[200];
	snprintf(buff, sizeof(buff), "%s:%d", "127.0.0.1", 10010);
	ctxs_[buff] = r0;
	snprintf(buff, sizeof(buff), "%s:%d", "127.0.0.1", 10020);
	ctxs_[buff] = r1;
	snprintf(buff, sizeof(buff), "%s:%d", "10.2.72.23", 10010);
	ctxs_[buff] = r2;

	for (map<string, rds_c_t>::iterator it = ctxs_.begin(); it != ctxs_.end(); ++it) {
		log_->trace("%s-->%s %p %d", fun, it->first.c_str(), it->second.c, it->second.st);
	}




}

void RedisContext::attach(redisAsyncContext *c)
{
	const char *fun = "RedisContext::attach";
	log_->info("%s-->attach c:%p loop:%p", fun, c, loop_);
	redisLibevAttach(loop_, c, (void *)this);
}

void RedisContext::hash_rcx(const std::vector<std::string> &hash, std::vector<redisAsyncContext *> &rcxs)
{
	for (map<string, rds_c_t>::iterator it = ctxs_.begin(); it != ctxs_.end(); ++it) {
		rcxs.push_back(it->second.c);
	}
}



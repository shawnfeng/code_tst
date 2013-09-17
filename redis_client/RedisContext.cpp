#include "RedisContext.h"
using namespace std;
static void connectCallback(const redisAsyncContext *c, int status) {
	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	RedisEvent *rev = (RedisEvent *)re->data;

	//rcx->log()->info("connectCallback c:%p", c);

	if (status != REDIS_OK) {
		rev->log()->error("connectCallback c:%p %s", c, c->errstr);

		return;
	} else {
		rev->log()->info("connectCallback c:%p ok", c);
	}

}

static void disconnectCallback(const redisAsyncContext *c, int status) {

	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	RedisEvent*rev = (RedisEvent *)re->data;

	//rcx->log()->info("disconnectCallback c:%p", c);
	if (status != REDIS_OK) {
		rev->log()->error("disconnectCallback c:%p %s", c, c->errstr);
		return;
	} else {
		rev->log()->info("disconnectCallback c:%p ok", c);
	}

}



void RedisContext::update_ends(std::vector< std::pair<std::string, int> > &ends)
{
	const char *fun = "RedisContext::update_ends";
	redisAsyncContext *c0 = redisAsyncConnect("127.0.0.1", 10010);
	redisAsyncContext *c1 = redisAsyncConnect("127.0.0.1", 10020);
	redisAsyncContext *c2 = redisAsyncConnect("10.2.72.23", 10010);
	//redisAsyncContext *c2 = redisAsyncConnect("127.0.0.1", 10030);

	c0->ev.data = NULL;
	c1->ev.data = NULL;
	c2->ev.data = NULL;
		

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
	//test = c1;

	rds_c_t r0, r1, r2;
	r0.c = c0; r0.st = rds_c_t::SYN;
	r1.c = c1; r1.st = rds_c_t::SYN;
	r2.c = c2; r2.st = rds_c_t::SYN;

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



	re_->attach(c0, connectCallback, disconnectCallback);
	re_->attach(c1, connectCallback, disconnectCallback);
	re_->attach(c2, connectCallback, disconnectCallback);

}

/*
void RedisContext::attach(redisAsyncContext *c)
{
	const char *fun = "RedisContext::attach";
	log_->info("%s-->attach c:%p loop:%p", fun, c, loop_);
	redisLibevAttach(loop_, c, (void *)this);
}
*/

void RedisContext::hash_rcx(const std::vector<std::string> &hash, std::set<redisAsyncContext *> &rcxs)
{
	const char *fun = "RedisContext::hash_rcx";
	for (map<string, rds_c_t>::iterator it = ctxs_.begin(); it != ctxs_.end(); ++it) {
		pair<set<redisAsyncContext *>::iterator, bool> rv = rcxs.insert(it->second.c);
		if (!rv.second) {
			log_->warn("%s-->duplicate context c:%p", fun, it->second.c);
		}
	}
}



#include <string>

#include "RedisEvent.h"

using namespace std;

struct cmd_arg_t {
	boost::mutex mux;
	boost::condition_variable cond;

	vector<string> vs;
};

static void connectCallback(const redisAsyncContext *c, int status) {
	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	//RedisEvent *rev = (RedisEvent *)re->data;

	//rcx->log()->info("connectCallback c:%p", c);

	userdata_t *u = (userdata_t *)ev_userdata(EV_DEFAULT);
	RedisEvent *rev = u->re();
	assert(rev);

	if (status != REDIS_OK) {
		rev->log()->error("connectCallback c:%p %s", c, c->errstr);
		u->clear(re->addr);
		return;
	} else {
		re->status = 1;
		rev->log()->info("connectCallback c:%p ok", c);
	}

}

static void disconnectCallback(const redisAsyncContext *c, int status) {

	redisLibevEvents *re = (redisLibevEvents *)c->ev.data;
	//RedisEvent *rev = (RedisEvent *)re->data;
	userdata_t *u = (userdata_t *)ev_userdata(EV_DEFAULT);
	RedisEvent *rev = u->re();
	assert(rev);

	if (status != REDIS_OK) {
		rev->log()->error("disconnectCallback c:%p %s", c, c->errstr);
		return;
	} else {
		rev->log()->info("disconnectCallback c:%p ok", c);
	}
	u->clear(re->addr);
}


static void async_cb (EV_P_ ev_async *w, int revents)
{
	//	puts("async_cb just used for the side effects");
}


static void l_release (EV_P)
{
	userdata_t *u = (userdata_t *)ev_userdata (EV_A);
	u->re()->unlock();
}

static void l_acquire (EV_P)
{
	userdata_t *u = (userdata_t *)ev_userdata (EV_A);
	u->re()->lock();
}


static void redis_cmd_cb(redisAsyncContext *c, void *r, void *data)
{
	// this point must alloc
	cflag_t *cf = (cflag_t *)data;
	assert(cf);
	userdata_t *u = (userdata_t *)ev_userdata(EV_DEFAULT);
	assert(u);
	assert(u->re());
	LogOut *log = u->re()->log();
	assert(log);

	if (log) log->trace("redis_cmd_cb-->cf=%p cf->f=%d cf->d=%p", cf, cf->f(), cf->d());

	if (cf->f() == 0 || cf->d() == NULL) {
		if (log) log->trace("redis_cmd_cb-->cf->f=%d return", cf->f());
		return;
	}


	// check ok, can use cf->d() pointer
	cmd_arg_t *carg = (cmd_arg_t *)cf->d();
	assert(carg);
	vector<string> &vs = carg->vs;


	if (cf->d_f() == 0) {
		if (log) log->trace("redis_cmd_cb-->reset 0 cf=%p cf->f=%d cf->d=%p", cf, cf->f(), cf->d());
		cf->reset();
	}

	redisReply *reply = (redisReply *)r;
	if (reply == NULL) {
		if (log) log->trace("redis_cmd_cb-->reply null");
		goto cond;
	}

	if (log) log->trace("redis_cmd_cb-->argv %s", reply->str);

	if (reply->str != NULL) {
		vs.push_back(reply->str);
	}

	//sleep(2);
 cond:
	if (cf->f() == 0) {
		boost::mutex::scoped_lock lock(carg->mux);
		carg->cond.notify_one();
		if (log) log->trace("redis_cmd_cb-->over cmd and notify_one");
	}

}


void RedisEvent::run()
{
	struct ev_loop *loop = loop_;

	l_acquire (EV_A);
	ev_run (EV_A_ 0);
	l_release (EV_A);

}

void RedisEvent::start()
{
	log_->info("RedisEvent::start-->loop:%p", loop_);
	ev_async_init (&async_w_, async_cb);
	ev_async_start (loop_, &async_w_);

	//log_->info("now associate this with the loop");
	ev_set_userdata (loop_, &ud_);
	ev_set_loop_release_cb (loop_, l_release, l_acquire);

	//log_->info("then create the thread running ev_run");

	boost::thread td(boost::bind(&RedisEvent::run, this));
	td.detach();
	log_->info("RedisEvent::start-->ok loop:%p", loop_);

}

// caller had been locked
void RedisEvent::connect(uint64_t addr)
{
	const char *fun = "RedisEvent::connect";

	char ip[50];
	int port;
	int64_ipv4(addr, ip, sizeof(ip), port);

	//	log_->info("%s-->connect ip:%s port:%d", ip, port);
	redisAsyncContext *c = redisAsyncConnect(ip, port);
	log_->info("%s-->connect ip:%s port:%d c:%p", fun, ip, port, c);
	if (c->err) {
		log_->error("%s-->%s", fun, c->errstr);
		return;
	}
	c->ev.data = NULL;

	ud_.insert(addr, c);

	redisLibevAttach(loop_, c, addr);
	redisAsyncSetConnectCallback(c, connectCallback);
	redisAsyncSetDisconnectCallback(c, disconnectCallback);

}

void RedisEvent::cmd(set<uint64_t> &addrs, const char *cs, int timeout, std::vector<std::string> &rv)
{
	//userdata *u = (userdata *)ev_userdata (loop_);
	const char *fun = "RedisEvent::cmd";
	ReqCount rcount(req_count_);

	log_->debug("%s-->size:%lu cmd:%s rcount=%d", fun, addrs.size(), cs, rcount.cn());
	if (addrs.empty()) {
		log_->warn("%s-->empty redis context cmd:%s", fun, cs);
		return;
	}

	userdata_t *u = &ud_;

	cmd_arg_t carg;
	cflag_t *cf = NULL;


	// ==========lock==========
	log_->trace("%s-->loop lock", fun);
	mutex_.lock();

	cf = u->get_cf();
	if (cf->f() || cf->d()) {
		log_->error("%s-->userdata have data!", fun);
	}

	int wsz = 0;
	for (set<uint64_t>::const_iterator it = addrs.begin();
	     it != addrs.end(); ++it) {

		redisAsyncContext *c = u->lookup(*it);
		if (NULL == c) {
			connect(*it);
		} else {
			redisLibevEvents *rd = (redisLibevEvents *)c->ev.data;
			assert(rd);
			log_->trace("%s-->c:%p e:%p st:%d", fun, *it, rd, rd->status);
			if (1 == rd->status) {
				redisAsyncCommand(c, redis_cmd_cb, cf, cs);
				wsz++;
			} else {
				log_->warn("%s-->connection is not ready c:%p", fun, c);
			}

		}

	}
	cf->set_f(wsz);
	cf->set_d((void *)&carg);


	ev_async_send (loop_, &async_w_);

	bool is_timeout = true;
	{
		// waiting
		boost::mutex::scoped_lock lock(carg.mux);  // must can get lock!

	mutex_.unlock(); // card.mux had beed locked, then release mutex_
	log_->trace("%s-->loop unlock", fun);
	// ==========unlock==========
	        log_->trace("%s-->condition wait %d", fun, timeout);
		is_timeout = !carg.cond.timed_wait(lock, boost::posix_time::milliseconds(timeout));
	}

	log_->trace("%s-->condition pass istimeout=%d", fun, is_timeout);
	if (is_timeout) {
		boost::mutex::scoped_lock lock(mutex_);
		log_->trace("%s-->cf=%p cf->f=%d cf->d=%p", fun, cf, cf->f(), cf->d());
		cf->reset();
	}
	// log out free lock
	if (is_timeout) {
		log_->warn("%s-->timeout cs:%s", fun, cs);
	}


	log_->info("%s-->size:%lu wsz:%d istimeout=%d cmd:%s", fun, addrs.size(), wsz, is_timeout, cs);

	rv.swap(carg.vs);

}


// =============================
void userdata_t::insert(uint64_t addr, redisAsyncContext *c)
{
	assert(ctxs_.find(addr) == ctxs_.end());
	re_->log()->info("userdata_t::insert addr:%lu c:%p", addr, c);
	ctxs_[addr] = c;
}

void userdata_t::clear(uint64_t addr)
{
	std::map<uint64_t, redisAsyncContext *>::iterator it = ctxs_.find(addr);
	assert(it != ctxs_.end());
	re_->log()->info("userdata_t::clear addr:%lu c:%p", addr, it->second);
	ctxs_.erase(it);
}

redisAsyncContext *userdata_t::lookup(uint64_t addr)
{
	std::map<uint64_t, redisAsyncContext *>::const_iterator it = ctxs_.find(addr);
	if (it == ctxs_.end()) {
		return NULL;
	} else {
		return it->second;
	}
}
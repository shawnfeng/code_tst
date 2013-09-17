#include <string>

#include "RedisEvent.h"

using namespace std;

struct cmd_arg_t {
	boost::mutex mux;
	boost::condition_variable cond;

	vector<string> vs;

	LogOut *log;

	cmd_arg_t(LogOut *lg) : log(lg) {}
};




static void async_cb (EV_P_ ev_async *w, int revents)
{
	//	puts("async_cb just used for the side effects");
}


static void l_release (EV_P)
{
	userdata_t *u = (userdata_t *)ev_userdata (EV_A);
	u->rc()->unlock();
}

static void l_acquire (EV_P)
{
	userdata_t *u = (userdata_t *)ev_userdata (EV_A);
	u->rc()->lock();
}


static void redis_cmd_cb(redisAsyncContext *c, void *r, void *data)
{
	cflag_t *cf = (cflag_t *)data;
	cmd_arg_t *carg = (cmd_arg_t *)cf->d();

	LogOut *log = carg->log;

	if (log) log->trace("redis_cmd_cb-->cf=%p cf->f=%d cf->d=%p", cf, cf->f(), cf->d());

	if (cf->f() == 0) {
		if (log) log->trace("redis_cmd_cb-->cf->f=%d return", cf->f());
		return;
	}

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
	loop_ = EV_DEFAULT;

	ev_async_init (&async_w_, async_cb);
	ev_async_start (loop_, &async_w_);

	log_.info("now associate this with the loop");
	ev_set_userdata (loop_, &ud_);
	ev_set_loop_release_cb (loop_, l_release, l_acquire);

	log_.info("then create the thread running ev_run");

	boost::thread td(boost::bind(&RedisEvent::run, this));
	td.detach();

}

void RedisEvent::attach(redisAsyncContext *c)
{
	redisLibevAttach(loop_, c);
}


void RedisEvent::cmd(std::vector<redisAsyncContext *>rcs, const char *c, int timeout)
{
	//userdata *u = (userdata *)ev_userdata (loop_);
	const char *fun = "RedisEvent::cmd";
	log_.debug("%s-->size:%lu cmd:%s", fun, rcs.size(), c);
	if (rcs.empty()) {
		log_.warn("%s-->empty redis context cmd:%s", fun, c);
		return;
	}

	userdata_t *u = &ud_;

	cmd_arg_t carg(&log_);
	cflag_t *cf = NULL;


	log_.trace("%s-->loop lock", fun);
	mutex_.lock();

	cf = u->get_cf();
	if (cf->f() || cf->d()) {
		log_.error("%s-->userdata have data!", fun);
	}
	cf->set_f((int)rcs.size());
	cf->set_d((void *)&carg);

	for (vector<redisAsyncContext *>::const_iterator it = rcs.begin();
	     it != rcs.end(); ++it) {
		redisAsyncCommand(*it, redis_cmd_cb, cf, c);
	}

	ev_async_send (loop_, &async_w_);

	bool is_timeout = true;
	{
		// waiting
		boost::mutex::scoped_lock lock(carg.mux);  // must can get lock!
	log_.trace("%s-->loop unlock", fun);
	mutex_.unlock(); // card.mux had beed locked, then release mutex_

	        log_.trace("%s-->condition wait %d", fun, timeout);
		is_timeout = !carg.cond.timed_wait(lock, boost::posix_time::milliseconds(timeout));
	}

	log_.trace("%s-->condition pass istimeout=%d", fun, is_timeout);
	if (is_timeout) {
		boost::mutex::scoped_lock lock(mutex_);
		//log_.trace("%s-->cf=%p cf->f=%d cf->d=%p", fun, cf, cf->f(), cf->d());
		cf->reset();
	}
	// log out free lock
	if (is_timeout) {
		log_.warn("%s-->timeout c:%s", fun, c);
	}

	const vector<string> &vs = carg.vs;
	for (vector<string>::const_iterator it = vs.begin(); it != vs.end(); ++it) {
		log_.debug("=== %s ===", it->c_str());
	}


}





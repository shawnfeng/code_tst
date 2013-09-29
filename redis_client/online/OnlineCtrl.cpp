#include <openssl/sha.h>

#include <boost/lexical_cast.hpp>

#include "OnlineCtrl.h"

using namespace std;

static const char *online_ope = "/online.lua";
static const char *offline_ope = "/offline.lua";
static const char *get_session_info_ope = "/get_session_info.lua";
static const char *get_session_ope = "/get_session_ope.lua";


static bool check_sha1(const char *path, string &data, string &sha1)
{
	char buf[512];
	SHA_CTX s;
	unsigned char hash[20];

	FILE *fp = fopen(path,"rb");
	if (!fp) return false;

	SHA1_Init(&s);
	int size;
	while ((size=fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
		SHA1_Update(&s, buf, size);
		data.append(buf, size);
	} 
	SHA1_Final(hash, &s);
	fclose(fp);

	sha1.clear();
	char tmp[10] = {0};
	for (int i=0; i < 20; i++) {
		sprintf (tmp, "%.2x", (int)hash[i]);
		sha1 += tmp;
	}

	return true;
 
}


void OnlineCtrl::offline(long uid, const std::string &session)
{
  const char *fun = "OnlineCtrl::offline";
	vector<string> hash;
	RedisRvs rv;
	string data, sha1;
  // !!!!!!!!!!load 需要优化！！
  string path = sp_ + offline_ope;
	if (!check_sha1(path.c_str(), data, sha1)) {
		log_.error("error check sha1");
	}
	//log_.trace("data:%s sha1:%s", data.c_str(), sha1.c_str());
	int timeout = 100;

  vector<string> args;
  args.push_back("EVALSHA");
  args.push_back(sha1);

  args.push_back("2");
  args.push_back(boost::lexical_cast<string>(uid));
  args.push_back(session);

  rc_.cmd(rv, hash, timeout, args, data);

	log_.debug("%s-->uid:%ld session:%s rv.size:%lu", fun, uid, session.c_str(), rv.size());
	for (RedisRvs::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		log_.trace("%s-->uid:%ld session:%s type:%d,int:%ld,len:%d,str:%s",
               fun, uid, session.c_str(), it->type, it->integer, it->len, it->str.c_str());

	}


}


void OnlineCtrl::online(long uid,
			const string &session,
			const vector<string> &kvs)
{
  const char *fun = "OnlineCtrl::online";

	vector<string> hash;
	RedisRvs rv;
	string data, sha1;

  string path = sp_ + online_ope;
  if (!check_sha1(path.c_str(), data, sha1)) {
		log_.error("error check sha1");
	}

	//log_.trace("data:%s sha1:%s", data.c_str(), sha1.c_str());
	int timeout = 100;
	int stamp = time(NULL);

	//rc_.cmd(rv, hash, 100, "SCRIPT LOAD %s", data.c_str());
	//rc_.cmd(rv, hash, 100, "EVAL %s %d %s %s %d", data.c_str(), 3, "t0", "t1", stamp);
	//rc_.cmd(rv, hash, 100, "EVALSHA %s %d %s %s", sha1.c_str(), 2, "t0", "t1");

	size_t sz = kvs.size();

	if (sz < 2 || sz % 2 != 0) {
		log_.error("kvs % 2 0 size:%lu", sz);
		return;
	}
  
  vector<string> args;
  args.push_back("EVALSHA");
  args.push_back(sha1);

  args.push_back("3");
  args.push_back(boost::lexical_cast<string>(uid));
  args.push_back(session);
  args.push_back(boost::lexical_cast<string>(stamp));

  args.insert(args.end(), kvs.begin(), kvs.end());

  rc_.cmd(rv, hash, timeout, args, data);

	log_.debug("%s-->uid:%ld session:%s rv.size:%lu", fun, uid, session.c_str(), rv.size());
	for (RedisRvs::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		log_.trace("%s-->uid:%ld session:%s type:%d,int:%ld,len:%d,str:%s",
               fun, uid, session.c_str(), it->type, it->integer, it->len, it->str.c_str());

	}


}

void OnlineCtrl::get_sessions(long uid, vector<string> &sessions)
{
  const char *fun = "OnlineCtrl::get_sessions";
	vector<string> hash;
	RedisRvs rv;
	string data, sha1;

  string path = sp_ + get_session_ope;
  if (!check_sha1(path.c_str(), data, sha1)) {
		log_.error("error check sha1");
	}

	int timeout = 100;

  vector<string> args;
  args.push_back("EVALSHA");
  args.push_back(sha1);

  args.push_back("1");
  args.push_back(boost::lexical_cast<string>(uid));

  rc_.cmd(rv, hash, timeout, args, data);

	log_.debug("%s-->uid:%ld rv.size:%lu", fun, uid, rv.size());
	for (RedisRvs::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		log_.trace("%s-->uid:%ld type:%d,int:%ld,len:%d,str:%s",
               fun, uid, it->type, it->integer, it->len, it->str.c_str());

    if (it->type == REDIS_REPLY_STRING) {
      sessions.push_back(it->str);
    }
	}

}


void OnlineCtrl::get_session_info(long uid, const string &session, const vector<string> &ks,
                                  map<string, string> &kvs)
{
  const char *fun = "OnlineCtrl::get_session_info";

	vector<string> hash;
	RedisRvs rv;
	string data, sha1;

  string path = sp_ + get_session_info_ope;
  if (!check_sha1(path.c_str(), data, sha1)) {
		log_.error("error check sha1");
	}

	int timeout = 100;

  vector<string> args;
  args.push_back("EVALSHA");
  args.push_back(sha1);

  args.push_back("2");
  args.push_back(boost::lexical_cast<string>(uid));
  args.push_back(session);

  args.insert(args.end(), ks.begin(), ks.end());

  rc_.cmd(rv, hash, timeout, args, data);

	log_.debug("%s-->uid:%ld session:%s rv.size:%lu", fun, uid, session.c_str(), rv.size());
	for (RedisRvs::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		log_.trace("%s-->uid:%ld session:%s type:%d,int:%ld,len:%d,str:%s",
               fun, uid, session.c_str(), it->type, it->integer, it->len, it->str.c_str());

	}

}



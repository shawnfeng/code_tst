#include <openssl/sha.h>

#include <boost/lexical_cast.hpp>

#include "OnlineCtrl.h"

using namespace std;

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
/*
// !!! add error log
static void rc_cmd2(RedisClient &rc, const vector<string> &hash, int timeout,
		    const char *sha1, const char *sp,
		    long uid, const char *session, int stamp, const vector<string> &kvs,
		    RedisRvs &rv)
{
	if (kvs.size() != 2) return;

	rc.cmd(rv, hash, timeout, "EVAL %s %d %d %s %d %s %s",
	       sp, 3, uid, session, stamp,
	       kvs[0].c_str(), kvs[1].c_str()
	       );
	
}

static void rc_cmd4(RedisClient &rc, const vector<string> &hash, int timeout,
		    const char *sha1, const char *sp,
		    long uid, const char *session, int stamp, const vector<string> &kvs,
		    RedisRvs &rv)
{
	if (kvs.size() != 4) return;

	rc.cmd(rv, hash, timeout, "EVAL %s %d %d %s %d %s %s %s %s",
	       sp, 3, uid, session, stamp,
	       kvs[0].c_str(), kvs[1].c_str(),
	       kvs[2].c_str(), kvs[3].c_str()
	       );
	
}

typedef void (*rc_eval_cmd)(RedisClient &rc, const vector<string> &hash, int timeout,
			    const char *sha1, const char *sp,
			    long uid, const char *session, int stamp, const vector<string> &kvs,
			    RedisRvs &rv);

static rc_eval_cmd rc_eval_cmd_vc[] = {
	rc_cmd2,
	rc_cmd4,
};
*/


void OnlineCtrl::offline(long uid, const std::string &session)
{
	vector<string> hash;
	RedisRvs rv;
	string data, sha1;

  string path = sp_ + "/offline.lua";
	if (!check_sha1(path.c_str(), data, sha1)) {
		log_.error("error check sha1");
	}
	log_.trace("data:%s sha1:%s", data.c_str(), sha1.c_str());
	int timeout = 100;

  vector<string> args;
  args.push_back("EVAL");
  args.push_back(data);

  args.push_back("2");
  args.push_back(boost::lexical_cast<string>(uid));
  args.push_back(session);

  rc_.cmd(rv, hash, timeout, args);



  /*
	rc_.cmd(rv, hash, timeout, "EVAL %s %d %d %s",
		data.c_str(), 2, uid, session.c_str()
		);
  */
	log_.debug("****@@rv.size:%lu", rv.size());
	for (RedisRvs::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		log_.debug("@@type:%d,int:%ld,len:%d,str:%s", it->type, it->integer, it->len, it->str.c_str());
	}


}


void OnlineCtrl::online(long uid,
			const string &session,
			const vector<string> &kvs)
{


	vector<string> hash;
	RedisRvs rv;
	string data, sha1;

  string path = sp_ + "/online.lua";
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
  args.push_back("EVAL");
  args.push_back(data);

  args.push_back("3");
  args.push_back(boost::lexical_cast<string>(uid));
  args.push_back(session);
  args.push_back(boost::lexical_cast<string>(stamp));

  args.push_back("k0");
  args.push_back("v0");

  args.insert(args.end(), kvs.begin(), kvs.end());

  rc_.cmd(rv, hash, timeout, args);



  /*

	--sz /= 2;
	log_.trace("index:%lu", sz);
	if (sz >= sizeof(rc_eval_cmd_vc)/sizeof(rc_eval_cmd_vc[0])) {
		log_.error("bigsize size:%lu", sz);
		return;
	}


	rc_eval_cmd_vc[sz](rc_, hash, timeout,
		       sha1.c_str(), data.c_str(),
		       uid, session.c_str(), stamp, kvs,
		       rv);
		
  */
	log_.debug("@@rv.size:%lu", rv.size());
	for (RedisRvs::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		log_.debug("@@type:%d,int:%ld,len:%d,str:%s", it->type, it->integer, it->len, it->str.c_str());
	}


}




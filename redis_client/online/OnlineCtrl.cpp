#include <openssl/sha.h>

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



static LogOut g_log;


void OnlineCtrl::online(long uid,
			const string &session,
			const map<string, string> &kvs)
{


	vector<string> hash;
	RedisRvs rv;
	string data, sha1;
	if (!check_sha1("/home/code/code_tst/redis_client/online/online.lua", data, sha1)) {
		g_log.error("error check sha1");
	}

	g_log.trace("data:%s sha1:%s", data.c_str(), sha1.c_str());

	//rc_.cmd(rv, hash, 100, "SCRIPT LOAD %s", data.c_str());
	rc_.cmd(rv, hash, 100, "EVAL %s %d %s %s", data.c_str(), 2, "t0", "t1");
	//rc_.cmd(rv, hash, 100, "EVALSHA %s %d %s %s", sha1.c_str(), 2, "t0", "t1");


	g_log.debug("@@rv.size:%lu", rv.size());
	for (RedisRvs::const_iterator it = rv.begin(); it != rv.end(); ++it) {
		g_log.debug("@@type:%d,int:%ld,len:%d,str:%s", it->type, it->integer, it->len, it->str.c_str());
	}


}



int main (int argc, char **argv)
{
	g_log.debug("%u", sizeof(int));
	g_log.debug("%u", sizeof(long));
	g_log.debug("%u", sizeof(long long));
	g_log.debug("%u", sizeof(long long int));


	OnlineCtrl oc;

	long uid = 0;
	string session;
	map<string, string> kvs;
	

	sleep(1);
	oc.online(uid, session, kvs);
	g_log.debug("=================");
	sleep(1);
	oc.online(uid, session, kvs);

	pause();
	

	return 0;
}


#include <stdio.h>

#include <string>
#include <vector>
#include <set>


#include <zookeeper/zookeeper.h>

using namespace std;


void watcher(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx) {}

int get_cld(zhandle_t *zh, const char *path, set<string> &revs)
{
	struct String_vector cfg_info;
	int rc = zoo_get_children(zh, path, 0, &cfg_info);


	if (rc == ZOK) {
		char **dp = cfg_info.data;
		for (int i = 0; i < cfg_info.count; ++i) {
			char *chd = *dp++;
			//printf("%s/%s\n", path, chd);
			revs.insert(chd);
		}

		deallocate_String_vector(&cfg_info);
	} else {
		printf("ERROR:zoo_get_children get %d %s error!\n", rc, path);
		return 1;
	}


	return 0;

}


int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("less argv!\n");
		return 1;
	}

	const char *zkhost = argv[1];
	string path = argv[2];
	path += "/error";
	//path += "/config";
	

	zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
	zhandle_t *zh = zookeeper_init(zkhost, watcher, 10000, 0, 0, 0);
	if (!zh) {
		printf("ERROR:zk init error!\n");
		return 1;
	}

	set<string> revs;
	if (get_cld(zh, path.c_str(), revs)) {
		return 1;
	}

	set<string> err_addrs;
	for (set<string>::const_iterator it = revs.begin();
	     it != revs.end(); ++it) {

		string addr = path + "/" + *it;
		printf("%s\n", addr.c_str());
		if (get_cld(zh, addr.c_str(), err_addrs)) {
			printf("ERROR:get path %s error!\n", addr.c_str());
			continue;
		}

	}


	for (set<string>::const_iterator it = err_addrs.begin();
	     it != err_addrs.end(); ++it) {
		printf("ERROR:%s\n", it->c_str());
	}



	zookeeper_close(zh);
	return 0;
}

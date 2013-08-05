#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <string>
#include <vector>
#include <algorithm>

#include "zookeeper.h"


using namespace std;


void watcher(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx) {}

int recpath(zhandle_t *zh, const char *path)
{
	char pc[1024];
	char buffer[1024];
	int buflen = 0;
	int rc, i;
	struct String_vector chs; // memory leak?
	rc = zoo_get_children(zh, path, 0, &chs);

	if (rc != ZOK) {
		printf("ERROR: zk get children error!\n");
		return 1;
	}

	printf("==========path=%s, count=%d==========\n", path, chs.count);
	vector<string> vps;
	for (i = 0; i < chs.count; ++i) {
		snprintf(pc, sizeof(pc), "%s/%s", path, *chs.data++);
		vps.push_back(pc);

	}

	sort(vps.begin(), vps.end());

	for (i = 0; i < chs.count; ++i) {
		const char *p = vps.at(i).c_str();

		buflen = sizeof(buffer);
		struct Stat stat;
		rc = zoo_get(zh, p, 0, buffer, &buflen, &stat);
		buffer[buflen] = '\0';
		if (rc != ZOK) {
			printf("ERROR: zk get path:%s date error!\n", p);
			return 1;
		}

		printf("%s %s\n", p, buffer);

	}

	return 0;

}

int main(int argc, char **argv) {
	int rc;

	if (argc != 2) {
		printf("ERROR: input error!\n");
		return 1;
	}


	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);

	const char *zkhost = "UserZooKeeper1.d.xiaonei.com:2181,UserZooKeeper2.d.xiaonei.com:2181,UserZooKeeper3.d.xiaonei.com:2181,UserZooKeeper4.d.xiaonei.com:2181,UserZooKeeper5.d.xiaonei.com:2181";


	zhandle_t *zh = zookeeper_init(zkhost, watcher, 10000, 0, 0, 0);
	if (!zh) {
		printf("ERROR: zk init error!\n");
		return 1;
	}


	const char *path = NULL;

	path = argv[1];
	if (recpath(zh, path)) {
		printf("ERROR: zk rec path=%s!\n", path);
	}


	zookeeper_close(zh);
	return 0;
}


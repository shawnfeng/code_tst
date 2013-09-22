#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <string>
#include <vector>
#include <algorithm>

#include <zookeeper/zookeeper.h>

#include "Util.h"


using namespace std;

static LogOut g_log;

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
		g_log.error("ERROR: zk get children error!");
		return 1;
	}

	g_log.info("==========path=%s, count=%d==========", path, chs.count);
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
			g_log.error("ERROR: zk get path:%s date error!", p);
			return 1;
		}

		g_log.info("%s %s", p, buffer);

	}

	return 0;

}

int main(int argc, char **argv) {
	int rc;

	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);

	const char *zkhost = "127.0.0.1:4180,127.0.0.1:4181,127.0.0.1:4182";

	zhandle_t *zh = zookeeper_init(zkhost, watcher, 10000, 0, 0, 0);

	if (!zh) {
		g_log.error("ERROR: zk init error!");
		return 1;
	}

	sleep(20);


	zookeeper_close(zh);
	return 0;
}


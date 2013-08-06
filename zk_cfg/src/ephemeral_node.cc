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

	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);

	const char *zkhost = "127.0.0.1:4180,127.0.0.1:4181,127.0.0.1:4182";

	zhandle_t *zh = zookeeper_init(zkhost, watcher, 10000, 0, 0, 0);

	if (!zh) {
		printf("ERROR: zk init error!\n");
		return 1;
	}

	char path_buffer[1024];
	int path_buffer_len = sizeof(path_buffer);
	const char *path = "/ephemeral_node";
	rc = zoo_create(zh, path, NULL, -1,
			&ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL,
			path_buffer, path_buffer_len);

	printf("create path=%s\n", path_buffer);

	const clientid_t *cid = zoo_client_id(zh);
	printf("==========id=%ld, pass=%s\n==========", cid->client_id, cid->passwd);


	if (rc != ZOK) {
		printf("ERROR: zk create children error %d!\n", rc);
		return 1;
	}



	sleep(20);

	zookeeper_close(zh);
	return 0;
}


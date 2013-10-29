#include <stdio.h>

#include <string>
#include <vector>


#include <zookeeper/zookeeper.h>

using namespace std;

void watcher(zhandle_t *zzh, int type, int state, const char *path,
             void *watcherCtx) {}


int create_node(zhandle_t *zkh, const char *path, int flags, const string &value, string &true_path)
{
	const char *fun = "RedisCtrl::create_node";
	const char *v = NULL;
	int vl = -1;
	if (!value.empty()) {
		v = value.c_str();
		vl = (int)value.size();
	}

	char buf[1024];

	int rv = zoo_create(zkh, path, v, vl,
			    &ZOO_OPEN_ACL_UNSAFE, flags,
			    buf, sizeof(buf));

	if (rv != ZOK) {
		printf("%s create node err path:%s flags:%d value%s rv:%d\n",
		       fun, path, flags, value.c_str(), rv
		       );
		return 1;
	}

	true_path = buf;
	printf("%s-->create zk node path:%s truepath:%s flags:%d value:%s\n",
	       fun, path, true_path.c_str(), flags, value.c_str()
	       );
	return 0;

}


int main()
{
	const char *zkhost = "10.4.25.15:4180,10.4.25.15:4181,10.4.25.15:4182";
	zhandle_t *zh = zookeeper_init(zkhost, watcher, 10000, 0, 0, 0);
	if (!zh) {
		printf("ERROR: zk init error!\n");
		return 1;
	}

	const char *master_ips[] = {
		"10.4.25.15",
		"10.4.25.16",
		"10.4.25.45",
		"10.4.25.46",
	};

	const char *slave_ips[] = {
		"10.4.25.16",
		"10.4.25.15",
		"10.4.25.46",
		"10.4.25.45",
	};


	vector<string> master;
	vector<string> slave;

	char buff[1024];
	for (int i = 0; i < (int)sizeof(master_ips)/sizeof(master_ips[0]); ++i) {
		for (int j = 0; j < 6; ++j) {
			snprintf(buff, sizeof(buff), "%s:%d", master_ips[i], 10020+j);
			master.push_back(buff);
		}

	}

	for (int i = 0; i < (int)sizeof(slave_ips)/sizeof(slave_ips[0]); ++i) {
		for (int j = 0; j < 6; ++j) {
			snprintf(buff, sizeof(buff), "%s:%d", slave_ips[i], 10026+j);
			slave.push_back(buff);
		}
	}


	//const vector<string> &rds = master;
	const vector<string> &rds = slave;

	for (int i = 0; i < (int)rds.size(); ++i) {
		snprintf(buff, sizeof(buff), "%s%d/%s", "/tx/online/config/cluster", i, rds.at(i).c_str());
		//snprintf(buff, sizeof(buff), "%s%d", "/tx/online/config/cluster", i);
		string true_path;

		create_node(zh, buff, 0, "", true_path);
		printf("create %s:%s\n", buff, true_path.c_str());

	}

	return 0;
}

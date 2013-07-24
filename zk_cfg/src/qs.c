#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zookeeper/zookeeper.h>
#include <zookeeper/zookeeper_log.h>

void QueryServer_watcher_g(zhandle_t* zh, int type, int state,
			   const char* path, void* watcherCtx)
{
	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
			printf("[[[QueryServer]]] Connected to zookeeper service successfully!\n");
		} else if (state == ZOO_EXPIRED_SESSION_STATE) { 
			printf("Zookeeper session expired!\n");
		}
	}  
}

void QueryServer_string_completion(int rc, const char *name, const void *data)
{
	fprintf(stderr, "[%s]: rc = %d\n", (char*)(data==0?"null":data), rc);
	if (!rc) {
		fprintf(stderr, "\tname = %s\n", name);
	}
}

void QueryServer_accept_query()
{
	printf("QueryServer is running...\n");
}

int main(int argc, const char *argv[])
{
	const char* host = "127.0.0.1:4180,127.0.0.1:4181,127.0.0.1:4182";

    int timeout = 30000;
    
    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    zhandle_t* zkhandle = zookeeper_init(host,
					 QueryServer_watcher_g, timeout, 0, "hello zookeeper.", 0);
    if (zkhandle == NULL) {
	    fprintf(stderr, "Error when connecting to zookeeper servers...\n");
	    exit(EXIT_FAILURE);
    }

    // struct ACL ALL_ACL[] = {{ZOO_PERM_ALL, ZOO_ANYONE_ID_UNSAFE}};
    // struct ACL_vector ALL_PERMS = {1, ALL_ACL};
    /*
    int ret = zoo_acreate(zkhandle, "/QueryServer", "alive", 5,
    			  &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL,
			  QueryServer_string_completion, "zoo_acreate");
    */

    //struct Id perm_my = {"digest", "aa:MWQ5NTVjMDY5MDkzYmI4NWIyNGViZjE1ZDMwOWNjZmExY2ZhMzkzNCAgYQo="};
    //struct Id perm_my = {"digest", "aa:OWQ0ZTFlMjNiZDViNzI3MDQ2YTllM2I0YjdkYjU3YmQ4ZDZlZTY4NA=="};
    struct Id perm_my = {"digest", "hxf:w1m9wyeJ9cStmkU1oNDka0uisFM="}; // hxf:123
    struct ACL READ_ONLY_ACL[] = {{ZOO_PERM_READ, perm_my}};
    struct ACL_vector READ_ONLY = {1, READ_ONLY_ACL};

    int ret = zoo_acreate(zkhandle, "/QueryServer", "alive", 5,
    			  &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL,
			  QueryServer_string_completion, "zoo_acreate");

    if (ret) {
	    fprintf(stderr, "Error %d for %s\n", ret, "acreate");
	    exit(EXIT_FAILURE);
    }
    struct Id perm_my0 = {"digest", "hxf0:JtsEfbVJiiQNmTEToGFnTJ5uSQA="};  // hxf0:1234
    struct ACL READ_ONLY_ACL0[] = {{ZOO_PERM_READ, perm_my0}};
    struct ACL_vector READ_ONLY0 = {1, READ_ONLY_ACL0};

    ret = zoo_acreate(zkhandle, "/QueryServer0", "eeivepddd", strlen("eeivepddd"),
    			  &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL,
			  QueryServer_string_completion, "zoo_acreate 2");

    if (ret) {
	    fprintf(stderr, "Error %d for %s\n", ret, "acreate");
	    exit(EXIT_FAILURE);
    }


    char buffer[512] = "empty";
    int buflen= sizeof(buffer);
    printf("buflen=%d\n", buflen);

    struct Stat stat;
    int rc = 0;
    rc = zoo_add_auth(zkhandle,"digest","hxf:123",strlen("hxf:123"),0,0);
    fprintf(stdout, "POS 0:rc=%d\n", rc);

    rc = zoo_add_auth(zkhandle,"digest","hxf0:1234",strlen("hxf0:1234"),0,0);
    fprintf(stdout, "POS 1:rc=%d\n", rc);

    rc = zoo_get(zkhandle, "/QueryServer", 0, buffer, &buflen, &stat);
    fprintf(stdout, "POS 2:rc=%d\n", rc);
    printf("=====%s\n", buffer);

    buffer[0] = '\0';
    buflen= sizeof(buffer);
    printf("buflen=%d\n", buflen);
    rc = zoo_get(zkhandle, "/QueryServer0", 0, buffer, &buflen, &stat);
    fprintf(stdout, "POS 3:rc=%d\n", rc);
    printf("=====%s\n", buffer);



    do {
	    // 模拟 QueryServer 对外提供服务.
	    // 为了简单起见, 我们在此调用一个简单的函数来模拟 QueryServer.
	    // 然后休眠 5 秒，程序主动退出(即假设此时已经崩溃).
	    QueryServer_accept_query();
	    sleep(5);
    } while(false);

    zookeeper_close(zkhandle);
}


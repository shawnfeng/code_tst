#include <string>


#include <zookeeper/zookeeper.h>

using namespace std;

int create_node(const char *path, int flags, const string &value, string &true_path)
{
  const char *fun = "RedisCtrl::create_node";
  const char *v = NULL;
  int vl = -1;
  if (!value.empty()) {
    v = value.c_str();
    vl = (int)value.size();
  }

  char buf[1024];

  int rv = zoo_create(zkh_, path, v, vl,
                      &ZOO_OPEN_ACL_UNSAFE, flags,
                      buf, sizeof(buf));

  if (rv != ZOK) {
    log_->error("%s create node err path:%s flags:%d value%s rv:%d",
                fun, path, flags, value.c_str(), rv
                );
    return 1;
  }

  true_path = buf;
  log_->info("%s-->create zk node path:%s truepath:%s flags:%d value:%s",
             fun, path, true_path.c_str(), flags, value.c_str()
             );
  return 0;

}


int main()
{
  return 0;
}

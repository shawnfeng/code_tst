#include "RedisClient.h"

using namespace std;

void RedisClient::cmd(RedisRvs &rv, const std::vector<std::string> &hash,
                      int timeout, const std::vector<std::string> &args,
                      const std::string &lua_code
                      )
{
  const char *fun = "RedisClient::cmd";
	set<uint64_t> addrs;
	rcx_.hash_addr(hash, addrs);

  string cmd;
  for (size_t i = 0; i < args.size(); ++i) {
    cmd.append(args[i]);
    cmd.append(" ");
  }

  log_.debug("%s-->cmd:%s", fun, cmd.c_str());

  re_.cmd(rv, addrs, timeout, args, lua_code, false);


}

void RedisClient::cmd(const std::vector<long> &hash, const char *c, int timeout, std::vector<std::string> &rv)
{

}


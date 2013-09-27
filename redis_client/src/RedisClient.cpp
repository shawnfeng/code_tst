#include "RedisClient.h"

using namespace std;

void RedisClient::cmd(RedisRvs &rv, const std::vector<std::string> &hash,
                      int timeout, const std::vector<std::string> &args,
                      const std::string &lua_code
                      )
{
	set<uint64_t> addrs;
	rcx_.hash_addr(hash, addrs);


  const char **argv = new const char *[args.size()];

  for (size_t i = 0; i < args.size(); ++i) {
    argv[i] = args[i].c_str();
  }

  re_.cmd(rv, addrs, timeout, args.size(), argv, NULL, lua_code);

  delete [] argv;

}

void RedisClient::cmd(const std::vector<long> &hash, const char *c, int timeout, std::vector<std::string> &rv)
{

}


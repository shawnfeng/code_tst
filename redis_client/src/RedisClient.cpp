#include <boost/scoped_ptr.hpp>
#include "RedisClient.h"

using namespace std;

class redis_arg_t {
public:

  const char **argv;
  size_t *argvlen;

  redis_arg_t(size_t sz) : argv(NULL), argvlen(NULL)
  {
    argv = new const char *[sz];
    argvlen = new size_t[sz];
  }

  ~redis_arg_t()
  {
    if (argv) delete [] argv;
    if (argvlen) delete argvlen;
  }

};

void RedisClient::cmd(RedisRvs &rv, const std::vector<std::string> &hash,
                      int timeout, const std::vector<std::string> &args,
                      const std::string &lua_code
                      )
{
  const char *fun = "RedisClient::cmd";
	set<uint64_t> addrs;
	rcx_.hash_addr(hash, addrs);

  boost::scoped_ptr<redis_arg_t>inarg(new redis_arg_t(args.size()));

  const char **argv = inarg->argv;
  size_t *argvlen = inarg->argvlen;

  string cmd;
  for (size_t i = 0; i < args.size(); ++i) {
    argv[i] = args[i].c_str();
    argvlen[i] = args[i].size();
    cmd.append(args[i]);
    cmd.append(" ");
  }

  log_.debug("%s-->cmd:%s", fun, cmd.c_str());

  re_.cmd(rv, addrs, timeout, args.size(), argv, argvlen, lua_code);


}

void RedisClient::cmd(const std::vector<long> &hash, const char *c, int timeout, std::vector<std::string> &rv)
{

}


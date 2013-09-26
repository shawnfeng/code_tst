EVPATH=/home/code/redis_client/libev
HIREDIS_PATH=/home/code/redis_client/hiredis
#HIREDIS_PATH_AD=$(HIREDIS_PATH)/adapters
ZKPATH=/home/code/rp/zookeeper/include

# ============
CC=g++
CCFLAGS=-c -Wall -g -fPIC -I$(EVPATH)/include -I$(HIREDIS_PATH) -I$(ZKPATH) -I../src
#AR=ar
#ARFLAGS=rcs

RPATH_EV=$(EVPATH)/lib
RPATH_HIREDIS=$(HIREDIS_PATH)
PATH_ZK=/home/code/rp/zookeeper/lib

LINK_FLAG=-lboost_thread -lcrypto
LINK_FLAG:=$(LINK_FLAG) -L$(RPATH_EV) -lev -Wl,-rpath,$(PATH_EV)
LINK_FLAG:=$(LINK_FLAG) -L$(RPATH_HIREDIS) -lhiredis -Wl,-rpath,$(RPATH_HIREDIS)
LINK_FLAG:=$(LINK_FLAG) -L$(PATH_ZK) -lzookeeper_mt -Wl,-rpath,$(PATH_ZK)

TEST_LINK_FLAG:= -L../src -lredis_client $(LINK_FLAG)

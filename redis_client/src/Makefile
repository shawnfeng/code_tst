EVPATH=/home/code/redis_client/libev
HIREDIS_PATH=/home/code/redis_client/hiredis
#HIREDIS_PATH_AD=$(HIREDIS_PATH)/adapters
ZKPATH=/home/code/rp/zookeeper/include

# ============
CC=g++
CCFLAGS=-c -Wall -g -fPIC -I$(EVPATH)/include -I$(HIREDIS_PATH) -I$(ZKPATH)
#AR=ar
#ARFLAGS=rcs

RPATH_EV=$(EVPATH)/lib
RPATH_HIREDIS=$(HIREDIS_PATH)
PATH_ZK=/home/code/rp/zookeeper/lib

LINK_FLAG=-lboost_thread
LINK_FLAG:=$(LINK_FLAG) -L$(RPATH_EV) -lev -Wl,-rpath,$(PATH_EV)
LINK_FLAG:=$(LINK_FLAG) -L$(RPATH_HIREDIS) -lhiredis -Wl,-rpath,$(RPATH_HIREDIS)
LINK_FLAG:=$(LINK_FLAG) -L$(PATH_ZK) -lzookeeper_mt -Wl,-rpath,$(PATH_ZK)


libredis_client.a: RedisClient.o RedisEvent.o RedisHash.o Util.o libev.o
	$(AR) $(ARFLAGS) $@ $^

# src client

RedisClient.o: RedisClient.cpp RedisClient.h Util.h libev.h
	$(CC) $(CCFLAGS) RedisClient.cpp

RedisEvent.o: RedisEvent.cpp RedisEvent.h Util.h libev.h
	$(CC) $(CCFLAGS) RedisEvent.cpp


RedisHash.o: RedisHash.cpp RedisHash.h RedisEvent.h Util.h libev.h
	$(CC) $(CCFLAGS) RedisHash.cpp

Util.o: Util.cpp Util.h
	$(CC) $(CCFLAGS) Util.cpp

libev.o: libev.cpp libev.h RedisEvent.h Util.h
	$(CC) $(CCFLAGS) libev.cpp



.PHONY: clean cleanall
clean:
	rm -f *.o
cleanall:
	rm -f *.o libredis_client.a
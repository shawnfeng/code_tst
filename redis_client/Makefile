EVPATH=/home/code/redis_client/libev
HIREDIS_PATH=/home/code/redis_client/hiredis
#HIREDIS_PATH_AD=$(HIREDIS_PATH)/adapters
ZKPATH=/home/code/rp/zookeeper/include

# ============
CC=g++
CCFLAGS=-c -Wall -g -fPIC -I$(EVPATH)/include -I$(HIREDIS_PATH) -I$(ZKPATH)

RPATH_EV=$(EVPATH)/lib
RPATH_HIREDIS=$(HIREDIS_PATH)
PATH_ZK=/home/code/rp/zookeeper/lib

LINK_FLAG=-lboost_thread
LINK_FLAG:=$(LINK_FLAG) -L$(RPATH_EV) -lev -Wl,-rpath,$(PATH_EV)
LINK_FLAG:=$(LINK_FLAG) -L$(RPATH_HIREDIS) -lhiredis -Wl,-rpath,$(RPATH_HIREDIS)
LINK_FLAG:=$(LINK_FLAG) -L$(PATH_ZK) -lzookeeper_mt -Wl,-rpath,$(PATH_ZK)


all: test_client
demo: 1st example-libev demo zkdemo

1st: 1st.o
	$(CC) -o 1st -Wl,-rpath $(RPATH_EV) 1st.o -L$(RPATH_EV) -lev

# -Wl,-rpath path可以用: 分隔
example-libev: example-libev.o
	$(CC) -o example-libev -Wl,-rpath,$(RPATH_EV) -Wl,-rpath,$(RPATH_HIREDIS) example-libev.o -L$(RPATH_HIREDIS) -L$(RPATH_EV) -lev -lhiredis

demo: demo.o
	$(CC) -o demo -Wl,-rpath,$(RPATH_EV) -Wl,-rpath,$(RPATH_HIREDIS) demo.o -L$(RPATH_HIREDIS) -L$(RPATH_EV) -lev -lhiredis -lpthread

zkdemo: zkdemo.o Util.o
	$(CC) -o zkdemo zkdemo.o Util.o $(LINK_FLAG)



1st.o: 1st.c
	$(CC) $(CCFLAGS) 1st.c

example-libev.o: example-libev.cc
	$(CC) $(CCFLAGS) example-libev.cc

demo.o: demo.cc demo.h
	$(CC) $(CCFLAGS) demo.cc

zkdemo.o: zkdemo.cc Util.h
	$(CC) $(CCFLAGS) zkdemo.cc


# src client
test_client: RedisClient.o test_client.o RedisHash.o RedisEvent.o Util.o libev.o
	$(CC) -o test_client test_client.o RedisClient.o RedisHash.o RedisEvent.o Util.o libev.o $(LINK_FLAG)

test_client.o: test_client.cpp RedisClient.h RedisEvent.h RedisHash.h Util.h libev.h
	$(CC) $(CCFLAGS) test_client.cpp

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
	rm -f *.o 1st example-libev demo test_client zkdemo
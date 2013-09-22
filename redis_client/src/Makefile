include ../Makefile.mk

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
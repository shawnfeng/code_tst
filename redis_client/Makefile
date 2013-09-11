EVPATH=/home/code/redis_client/libev


# ============
CC=gcc
CCFLAGS=-c -Wall -g -fPIC -I$(EVPATH)/include

RPATH=$(EVPATH)/lib


1st: 1st.o
	$(CC) -o 1st -Wl,-rpath $(RPATH) 1st.o -L$(RPATH) -lev

1st.o: 1st.c
	$(CC) $(CCFLAGS) 1st.c

.PHONY: clean cleanall
clean:
	rm -f *.o
cleanall:
	rm -f *.o 1st
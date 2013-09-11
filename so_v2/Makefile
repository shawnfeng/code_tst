
SO_LINK_NAME=libhello.so
SO_SONAME=$(SO_LINK_NAME).0
SO_REAL_NAME=$(SO_SONAME).0.0

MAINEXE=hw

CC=gcc
CCFLAGS=-c -Wall -g -fPIC
SHAREFLAG=
LINKFLAG=-Wl,-soname,$(SO_SONAME)

BECLEAN=*.o *~

all:$(SO_REAL_NAME) $(MAINEXE)

$(MAINEXE):main.o
	$(CC) -o $(MAINEXE) main.o  -shared -lhello -L. # -static  -lhello -L.

main.o:main.cpp hello.h
	$(CC) $(CCFLAGS) main.cpp

hello.o:hello.cpp hello.h
	$(CC) $(CCFLAGS) hello.cpp

$(SO_REAL_NAME):hello.o
	$(CC) -o $(SO_REAL_NAME) $(SHAREFLAG) $(LINKFLAG) hello.o


.PHONY: clean cleanall
clean:
	rm -f $(BECLEAN)
cleanall:
	rm -f $(SO_REAL_NAME) $(MAINEXE) $(BECLEAN)
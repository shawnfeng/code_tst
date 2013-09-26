include ../Makefile.mk

test_online: OnlineCtrl.o
	$(CC) -o test_online OnlineCtrl.o $(TEST_LINK_FLAG)


OnlineCtrl.o: OnlineCtrl.cpp OnlineCtrl.h
	$(CC) $(CCFLAGS) OnlineCtrl.cpp




.PHONY: clean cleanall
clean:
	rm -f *.o
cleanall:
	rm -f *.o test_online
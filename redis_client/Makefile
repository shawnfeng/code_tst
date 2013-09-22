all: base_lib test

base_lib:
	cd src && $(MAKE)

test: base_lib
	cd test && $(MAKE)



.PHONY: clean cleanall
clean:
	cd src && $(MAKE) $@
	cd test && $(MAKE) $@
cleanall:
	cd src && $(MAKE) $@
	cd test && $(MAKE) $@

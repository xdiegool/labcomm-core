.PHONY: all

all:
	echo To be done...
	$(MAKE) -C twoway all

.PHONY: test
test:
	echo More to be done...
	cd simple ; sh compile.sh && sh run.sh
	$(MAKE) -C twoway test 
	$(MAKE) -C duck_typing test

.PHONY: clean distclean
clean distclean:
	echo To be done...
	$(MAKE) -C twoway clean
	$(MAKE) -C duck_typing clean

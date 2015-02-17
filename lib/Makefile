.PHONY: all
all:
	cd c ; make
	cd csharp ; make
	cd java ; make

.PHONY: test
test:
	$(MAKE) -C c test

.PHONY: clean
clean:
	$(MAKE) -C c clean
	$(MAKE) -C csharp clean
	$(MAKE) -C java clean
	$(MAKE) -C python clean

.PHONY: distclean
distclean:
	$(MAKE) -C c distclean
	$(MAKE) -C csharp distclean
	$(MAKE) -C java  distclean
	$(MAKE) -C python distclean

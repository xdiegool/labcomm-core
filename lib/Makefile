all:
	cd c ; make
	cd csharp ; make
	cd java ; make

test:
	$(MAKE) -C c test

clean:
	cd c ; make clean
	cd csharp ; make clean
	cd java ; make clean
	$(MAKE) -C python clean

distclean:
	cd c ; make distclean
	cd csharp ; make clean
	cd java ; make clean
	$(MAKE) -C python clean

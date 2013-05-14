all:
	cd c ; make
	cd csharp ; make
	cd java ; make

test:
	$(MAKE) -C c -e run-test

clean:
	cd c ; make clean
	cd csharp ; make clean
	cd java ; make clean

SUBDIRS=compiler lib test examples
export LABCOMM_JAR=$(shell pwd)/compiler/labComm.jar
export LABCOMM=java -jar $(LABCOMM_JAR) 

all: $(SUBDIRS:%=make-%)

.PHONY: make-compiler
make-compiler:
	cd compiler ; ant jar

.PHONY: make-%
make-%:
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $* -e 

.PHONY: test
test: $(SUBDIRS:%=test-%)

.PHONY: test-compiler
test-compiler:

.PHONY: test-%
test-%:
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $* -e test

.PHONY: clean
clean: $(SUBDIRS:%=clean-%)

.PHONY: clean-compiler
clean-compiler:
	cd compiler ; ant clean

.PHONY: clean-%
clean-%:
	$(MAKE) -C $* -e clean

.PHONY: distclean
distclean: $(SUBDIRS:%=distclean-%)

.PHONY: distclean-compiler
distclean-compiler:
	cd compiler ; ant clean

.PHONY: distclean-%
distclean-%:
	$(MAKE) -C $* -e distclean

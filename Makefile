SUBDIRS=compiler lib test examples
export LABCOMM_JAR=$(shell pwd)/compiler/labcomm_compiler.jar
export LABCOMM=java -jar $(LABCOMM_JAR) 

all: $(SUBDIRS:%=make-%)

.PHONY: make-%
make-%:
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $* -e 

.PHONY: test
test: $(SUBDIRS:%=test-%)

.PHONY: test-%
test-%:
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $* -e test

.PHONY: clean
clean: $(SUBDIRS:%=clean-%)

.PHONY: clean-%
clean-%:
	$(MAKE) -C $* -e clean

.PHONY: distclean
distclean: clean $(SUBDIRS:%=distclean-%)

.PHONY: distclean-%
distclean-%:
	$(MAKE) -C $* -e distclean

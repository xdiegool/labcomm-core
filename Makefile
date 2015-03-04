SUBDIRS=compiler lib test examples packaging
export LABCOMM_JAR=$(shell pwd)/compiler/labcomm2014_compiler.jar
export LABCOMM=java -jar $(LABCOMM_JAR) 

.PHONY: all
all: $(SUBDIRS:%=all-%)

.PHONY: all-%
all-%:
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $*

.PHONY: test
test: $(SUBDIRS:%=test-%)

.PHONY: test-%
test-%:
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $* test

.PHONY: clean
clean: $(SUBDIRS:%=clean-%)

.PHONY: clean-%
clean-%:
	$(MAKE) -C $* clean

.PHONY: distclean
distclean: clean $(SUBDIRS:%=distclean-%)

.PHONY: distclean-%
distclean-%:
	$(MAKE) -C $* distclean

.PHONY: srpm
srpm:
	make -C packaging $@

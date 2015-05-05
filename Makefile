SUBDIRS=compiler lib test examples packaging
export LABCOMM_JAR=$(shell pwd)/compiler/labcomm2014_compiler.jar
export LABCOMM=java -jar $(LABCOMM_JAR) 

UNAME_S=$(shell uname -s)

.PHONY: all
all: $(SUBDIRS:%=all-%)

.PHONY: all-%
all-%:
ifeq ($(UNAME_S),Darwin)
	DYLD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $*
else
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $*
endif

.PHONY: test
test: $(SUBDIRS:%=test-%)

.PHONY: test-%
test-%:
ifeq ($(UNAME_S),Darwin)
	DYLD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $* test
else
	LD_LIBRARY_PATH=`pwd`/lib/c $(MAKE) -C $* test
endif

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

SUBDIRS=c csharp java python

.PHONY: all
all: $(SUBDIRS:%=make-%)

.PHONY: make-%
make-%:
	$(MAKE) -C $*

.PHONY: test
test: $(SUBDIRS:%=test-%)

.PHONY: test-%
test-%:
	$(MAKE) -C $* test

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

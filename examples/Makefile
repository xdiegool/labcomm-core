SUBDIRS=duck_typing dynamic jgrafchart robot simple tcp \
	twoway user_types wiki_example

.PHONY: all

all:
	echo More to be done...
	$(MAKE) -C twoway all

.PHONY: test
test:
	echo More to be done...
	cd simple ; sh compile.sh && sh run.sh
	$(MAKE) -C  wiki_example test
	$(MAKE) -C duck_typing test
	$(MAKE) -C twoway test 

.PHONY: clean
clean: $(SUBDIRS:%=clean-%)
clean-%:
	$(MAKE) -C $* clean 

.PHONY: distclean
distclean: $(SUBDIRS:%=distclean-%)
distclean-%:
	$(MAKE) -C $* distclean 

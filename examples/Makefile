SUBDIRS=duck_typing dynamic jgrafchart robot simple tcp \
	twoway user_types wiki_example

.PHONY: all

all:
	echo More to be done...
	$(MAKE) -C twoway all

UNAME_S=$(shell uname -s)

.PHONY: test
test:
	echo More to be done...
ifeq ($(UNAME_S),Darwin)
	$(MAKE) -C user_types all 
else	
        # FIXME: cd simple ; sh compile.sh && sh run.sh
        # FIXME: $(MAKE) -C  wiki_example test
        # FIXME: $(MAKE) -C user_types test 
endif	
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

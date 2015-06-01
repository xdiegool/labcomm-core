SUBDIRS=duck_typing dynamic jgrafchart robot simple tcp \
	twoway wiki_example
# user_types # Disabled until typeefs are reimplemented in library

.PHONY: all

all:
	echo More to be done...
	$(MAKE) -C twoway all

UNAME_S=$(shell uname -s)

.PHONY: test
test:
	echo More to be done...
ifeq ($(UNAME_S),Darwin)
#	$(MAKE) -C user_types all 
else	
	cd simple ; sh compile.sh && sh run.sh
	$(MAKE) -C  wiki_example test
#	$(MAKE) -C user_types test 
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

VERSIONS=2006 2014

all:

%: 
	for v in $(VERSIONS) ; do $(MAKE) -C $${v} $@ || exit 1 ; done


.PHONY: distclean
distclean: 
	rm *.o *.so *.so.1 *.a
	for v in $(VERSIONS) ; do $(MAKE) -C $${v} distclean ; done


VERSIONS=2006 2014

all:

%: 
	for v in $(VERSIONS) ; do $(MAKE) -C $${v} $@ || exit 1 ; done

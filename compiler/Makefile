.PHONY: all
all: ant-all

.PHONY: test
test: ant-test

.PHONY: clean
clean: ant-clean
	rm -f *~

.PHONY: distclean
distclean: clean ant-distclean

.PHONY: ant-%
ant-%:
	ant $*

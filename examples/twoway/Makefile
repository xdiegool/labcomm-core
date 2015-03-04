UNAME_S=$(shell uname -s)

TARGETS=client server
LABCOMM_JAR=../../compiler/labcomm2014_compiler.jar
LABCOMM=java -jar $(LABCOMM_JAR) 

CFLAGS=-O3 -g -Wall -Werror -I../../lib/c/2014 -I. -Wno-unused-function

ifeq ($(UNAME_S),Darwin)
    CFLAGS+=-DLABCOMM_COMPAT=\"labcomm_compat_osx.h\" -DLABCOMM_OS_DARWIN=1  -Wno-tautological-compare
endif

all: $(TARGETS:%=gen/%)

test: all
	LD_LIBRARY_PATH=../../lib/c ./gen/server 2000 &
	LD_LIBRARY_PATH=../../lib/c ./gen/client localhost 2000 

gen/.dir:
	mkdir -p $@

.PRECIOUS: gen/%.o
gen/%.o: gen/%.c | gen/.dir
	$(CC) $(CFLAGS) -c -o $@ $< 

gen/%.o: %.c | gen/.dir
	$(CC) $(CFLAGS) -c -o $@ $< 

.PRECIOUS: gen/%.c gen/%.h
gen/%.c gen/%.h: %.lc | gen/.dir
	$(LABCOMM) --c=gen/$*.c --h=gen/$*.h $<

gen/client: client.c 
	$(CC) -o $@ $(CFLAGS) $^ -lpthread \
		-L../../lib/c -llabcomm2014

gen/server: server.c 
	$(CC) -o $@ $(CFLAGS) $^ -lpthread \
		-L../../lib/c -llabcomm2014 

.PHONY: clean distclean
clean distclean:
	rm -rf gen

gen/decimating.o: decimating.h
gen/decimating.o: gen/decimating_messages.h
gen/introspecting.o: introspecting.h
gen/introspecting.o: gen/introspecting_messages.h
gen/client.o: decimating.h
gen/client.o: gen/types.h
gen/client: gen/decimating.o 
gen/client: gen/decimating_messages.o
gen/client: gen/introspecting.o
gen/client: gen/introspecting_messages.o
gen/client: gen/types.o
gen/server: gen/types.o
gen/server: gen/decimating.o
gen/server: gen/decimating_messages.o
gen/server: gen/introspecting.o
gen/server: gen/introspecting_messages.o

TARGETS=client server
LABCOMM_JAR=../../compiler/labComm.jar
LABCOMM=java -jar $(LABCOMM_JAR) 

CFLAGS=-O3 -g -Wall -Werror -I../../lib/c -I. -lpthread

all: $(TARGETS:%=gen/%)

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

gen/client: client.c gen/types.o gen/decimating.o gen/decimating_messages.o
	$(CC) -o $@ $(CFLAGS) $^ -lpthread -L../../lib/c -llabcomm

gen/server: server.c gen/types.o gen/decimating.o gen/decimating_messages.o
	$(CC) -o $@ $(CFLAGS) $^ -lpthread -L../../lib/c -llabcomm

.PHONY: clean
clean:
	rm -rf gen

gen/decimating.o: decimating.h
gen/decimating.o: gen/decimating_messages.h
gen/client.o: decimating.h
gen/client.o: gen/types.h
gen/client: gen/decimating_messages.o

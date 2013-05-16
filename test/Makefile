TESTS=basic simple nested
LABCOMM_JAR=../compiler/labComm.jar
LABCOMM=java -jar $(LABCOMM_JAR) 

CFLAGS=-O3 -Wall -Werror

all:	

test: $(TESTS:%=test_%)
#	PYTHONPATH=../lib/python \
#		./test_encoder_decoder.py --labcomm="$(LABCOMM)" basic.lc

.PHONY: clean
clean:
	rm -rf gen

.PHONY: test_%
test_%: gen/%/signatures.py \
	gen/%/c_relay \
	gen/%/cs_relay.exe \
	gen/%/java_relay.class \
	gen/%/java_code 
	PYTHONPATH=../lib/python ./test_encoder_decoder.py \
		--signatures=gen/$*/signatures.py \
		--test tee gen/$*/testdata \
		--test gen/$*/c_relay /dev/stdin /dev/stdout \
		--test mono gen/$*/cs_relay.exe /dev/stdin /dev/stdout \
		--test java \\-cp gen/$*:../lib/java/labcomm.jar java_relay \
			    /dev/stdin /dev/stdout
.PRECIOUS: gen/%/.dir
gen/%/.dir:
	mkdir -p gen/$*
	touch $@

.PRECIOUS: gen/%/typeinfo
gen/%/typeinfo:	%.lc Makefile | gen/%/.dir
	$(LABCOMM) --typeinfo=$@ $<

.PRECIOUS: gen/%/signatures.py
gen/%/signatures.py: %.lc Makefile | gen/%/.dir
	$(LABCOMM) --python=$@ $<

# C relay test rules
.PRECIOUS: gen/%/c_code.h gen/%/c_code.c
gen/%/c_code.h gen/%/c_code.c: %.lc Makefile | gen/%/.dir
	$(LABCOMM) --c=gen/$*/c_code.c --h=gen/$*/c_code.h $<

.PRECIOUS: gen/%/c_relay.c
gen/%/c_relay.c: gen/%/typeinfo relay_gen_c.py Makefile
	./relay_gen_c.py $< > $@

.PRECIOUS: gen/%/c_relay
gen/%/c_relay: gen/%/c_relay.c gen/%/c_code.c Makefile
	$(CC) $(CFLAGS) -o $@ $< -I../lib/c -I. \
		-DLABCOMM_FD_OMIT_VERSION \
		-DLABCOMM_ENCODER_LINEAR_SEARCH \
		gen/$*/c_code.c \
		../lib/c/labcomm.c \
		../lib/c/labcomm_fd_*.c \
		../lib/c/labcomm_dynamic_buffer_writer.c 

# C# relay test rules
.PRECIOUS: gen/%/cs_code.cs
gen/%/cs_code.cs: %.lc Makefile | gen/%/.dir
	$(LABCOMM) --cs=$@ $<

.PRECIOUS: gen/%/cs_relay.cs
gen/%/cs_relay.cs:  gen/%/typeinfo relay_gen_cs.py Makefile
	./relay_gen_cs.py $< > $@

.PRECIOUS: gen/%/labcomm.dll
gen/%/labcomm.dll:
	ln -s ../../../lib/csharp/labcomm.dll $@

.PRECIOUS: gen/%/cs_relay.exe
gen/%/cs_relay.exe: gen/%/cs_relay.cs gen/%/cs_code.cs \
		    gen/%/labcomm.dll Makefile
	mcs -out:$@ $(filter %.cs, $^) -lib:../lib/csharp/ -r:labcomm

# Java relay test rules
.PRECIOUS: gen/%/java_code
gen/%/java_code: %.lc | gen/%/.dir
	mkdir -p $@
	$(LABCOMM) --java=$@ $<

.PRECIOUS: gen/%/java_relay.java
gen/%/java_relay.java: gen/%/typeinfo relay_gen_java.py Makefile
	./relay_gen_java.py $< > $@

.PRECIOUS: gen/%/java_relay.class
gen/%/java_relay.class: gen/%/java_relay.java gen/%/java_code Makefile
	javac -d gen/$*  -cp ../lib/java/labcomm.jar:gen/$*/java_code $<

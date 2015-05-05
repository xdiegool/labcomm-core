TESTS=basic simple nested ref
LABCOMM_JAR=../compiler/labcomm2014_compiler.jar
LABCOMM=java -jar $(LABCOMM_JAR)
MONO_PATH=$(shell realpath ../lib/csharp)
PYTHONPATH=$(shell realpath ../lib/python)

include ../lib/c/os_compat.mk

all:

test: $(TESTS:%=test_%) compiler_errors
#	PYTHONPATH=../lib/python \
#		./test_encoder_decoder.py --labcomm="$(LABCOMM)" basic.lc

.PHONY: clean distclean
clean distclean:
	rm -rf gen

.PHONY: test_% 
test_%: gen/%/signatures.py \
	gen/%/c_relay \
	gen/%/cs_relay.exe \
	gen/%/java_relay.class \
	gen/%/java_code
	PYTHONPATH=$(PYTHONPATH) MONO_PATH=$(MONO_PATH) \
		./test_encoder_decoder.py \
		--signatures=gen/$*/signatures.py \
		--test tee gen/$*/testdata \
		--test gen/$*/c_relay /dev/stdin /dev/stdout \
		--test mono gen/$*/cs_relay.exe /dev/stdin /dev/stdout \
		--test java \\-cp gen/$*:../lib/java/labcomm2014.jar java_relay \
			    /dev/stdin /dev/stdout

# test cases for compiler error checking
.PHONY: compiler_errors testErrorsOK testErrorsNOK

compiler_errors: testErrorsOK testErrorsNOK

# tests that should succeed
testErrorsOK: $(wildcard errors/correct/*.lc)
	./test_errors.py --labcomm="$(LABCOMM)" --testOK $^

# tests that should fail
testErrorsNOK: $(wildcard errors/incorrect/*.lc)
	./test_errors.py --labcomm="$(LABCOMM)" --testNOK $^

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
	$(CC) $(CFLAGS) -o $@ $< -I../lib/c/2014 -I. -L../lib/c \
		gen/$*/c_code.c -llabcomm2014

# C# relay test rules
.PRECIOUS: gen/%/cs_code.cs
gen/%/cs_code.cs: %.lc Makefile | gen/%/.dir
	$(LABCOMM) --cs=$@ $<

.PRECIOUS: gen/%/cs_relay.cs
gen/%/cs_relay.cs:  gen/%/typeinfo relay_gen_cs.py Makefile
	./relay_gen_cs.py $< > $@

.PRECIOUS: gen/%/cs_relay.exe
gen/%/cs_relay.exe: gen/%/cs_relay.cs gen/%/cs_code.cs Makefile
	mcs -out:$@ $(filter %.cs, $^) -lib:../lib/csharp/ -r:labcomm2014

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
	javac -d gen/$*  -cp ../lib/java/labcomm2014.jar:gen/$*/java_code $<

LCDIR=../..
LCCJAR=${LCDIR}/compiler/labcomm2014_compiler.jar# the LabComm compiler
LCLJAR=${LCDIR}/lib/java/labcomm2014.jar# the LabComm library

EXECUTABLES=example_encoder example_decoder \
	    Encoder.class Decoder.class \
	    ExampleEncoder.exe ExampleDecoder.exe

include ${LCDIR}/lib/c/os_compat.mk

GENDIR=gen

.PHONY: all cleanbuild clean distclean build run allall buildcs runwcs runjastadd

all: cleanbuild run

allall: clean build buildcs runwcs

test: allall

###############################################
### dependencies and parts ####################
###############################################

LCC=java -jar ${LCCJAR}
CLASSPATH=.:${LCLJAR}

${LCCJAR} :
	$MAKE -C ${LCDIR} make-compiler

${LCLJAR} :
	$MAKE -C ${LCDIR} labcomm2014.jar


cleanbuild: clean build

labcomm2014.dll:
	ln -sf ../../lib/csharp/labcomm2014.dll $@

ExampleEncoder.exe: ExampleEncoder.cs gen/test.cs labcomm2014.dll Makefile
	mcs -out:$@ $(filter %.cs, $^) -lib:../../lib/csharp/ -r:labcomm2014
	chmod a+x $@

ExampleDecoder.exe: ExampleDecoder.cs gen/test.cs labcomm2014.dll Makefile
	mcs -out:$@ $(filter %.cs, $^) -lib:../../lib/csharp/ -r:labcomm2014
	chmod a+x $@

build : 
	mkdir -p ${GENDIR}
	java -jar ${LCDIR}/compiler/labcomm2014_compiler.jar --java=${GENDIR} --c=${GENDIR}/test.c --h=${GENDIR}/test.h  --python=${GENDIR}/test.py --cs=${GENDIR}/test.cs test.lc 

	javac -cp ${LCDIR}/lib/java/labcomm2014.jar:. ${GENDIR}/*.java Encoder.java Decoder.java

	${CC} ${CFLAGS} ${LDFLAGS} -Wall -Werror -Wno-unused-function \
	    -I. -I${LCDIR}/lib/c/2014 -L${LCDIR}/lib/c \
	    -o example_encoder example_encoder.c ${GENDIR}/test.c \
	    -llabcomm2014 

	${CC} ${CFLAGS} ${LDFLAGS} -Wall -Werror -I . -I ${LCDIR}/lib/c/2014 -L${LCDIR}/lib/c \
	    -o example_decoder example_decoder.c ${GENDIR}/test.c \
	    -llabcomm2014 

buildcs: ExampleEncoder.exe ExampleDecoder.exe

run:
	@echo
	@echo "********************************************"
	@echo "***   ************ running example for version 2014   ***"
	@echo "********************************************"
	@echo

	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Encoder encoded_data_j

	@echo "************ running Java  decoder: *****************"
	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Decoder encoded_data_j

	@echo "************ running C decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_decoder encoded_data_j

	@echo "************ running python decoder (from wiki_example):"
	@PYTHONPATH=${LCDIR}/lib/python ../wiki_example/example_decoder.py encoded_data_j LabComm2014.v1

	@echo "************ running python decoder (with handlers):"
	@PYTHONPATH=${LCDIR}/lib/python:gen:. ./example_handlers.py encoded_data_j LabComm2014.v1

	@echo "************ running C encoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_encoder encoded_data_c

	@echo "************ running Java  decoder: *****************"
	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Decoder encoded_data_c

	@echo "************ running C decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_decoder encoded_data_c

	@echo "************ running python decoder (from wiki_example):"
	@PYTHONPATH=${LCDIR}/lib/python ../wiki_example/example_decoder.py encoded_data_c LabComm2014.v1

	@echo "************ running python decoder (with handlers):"
	@PYTHONPATH=${LCDIR}/lib/python:gen:. ./example_handlers.py encoded_data_j LabComm2014.v1

	@echo "************ running python encoder: *****************"
	@PYTHONPATH=${LCDIR}/lib/python:${GENDIR} ./example_encoder.py encoded_data_p LabComm2014.v1

	@echo "************ running Java  decoder: *****************"
	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Decoder encoded_data_p

	@echo "************ running C decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_decoder encoded_data_p

	@echo "************ running python decoder (from wiki_example):"
	PYTHONPATH=${LCDIR}/lib/python ../wiki_example/example_decoder.py encoded_data_p LabComm2014.v1

	@echo "************ running python decoder (with handlers):"
	@PYTHONPATH=${LCDIR}/lib/python:gen:. ./example_handlers.py encoded_data_j LabComm2014.v1


runwcs: build ExampleEncoder.exe ExampleDecoder.exe
	@echo
	@echo "********************************************"
	@echo "***   ************ running example for version 2014   ***"
	@echo "********************************************"
	@echo

	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Encoder encoded_data_j

	@echo "************ running Java  decoder: *****************"
	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Decoder encoded_data_j

	@echo "************ running C decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_decoder encoded_data_j

	@echo "************ running python decoder (from wiki_example):"
	@PYTHONPATH=${LCDIR}/lib/python ../wiki_example/example_decoder.py encoded_data_j LabComm2014.v1

	@echo "************ running C# decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ mono ./ExampleDecoder.exe encoded_data_j

	@echo "************ running C encoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_encoder encoded_data_c

	@echo "************ running Java  decoder: *****************"
	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Decoder encoded_data_c

	@echo "************ running C decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_decoder encoded_data_c

	@echo "************ running python decoder (from wiki_example):"
	@PYTHONPATH=${LCDIR}/lib/python ../wiki_example/example_decoder.py encoded_data_c LabComm2014.v1

	@echo "************ running C# decoder: *****************"
	@mono ./ExampleDecoder.exe encoded_data_c

	@echo "************ running python encoder: *****************"
	@PYTHONPATH=${LCDIR}/lib/python:${GENDIR} ./example_encoder.py encoded_data_p LabComm2014.v1

	@echo "************ running Java  decoder: *****************"
	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Decoder encoded_data_p

	@echo "************ running C decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_decoder encoded_data_p

	@echo "************ running python decoder (from wiki_example):"
	PYTHONPATH=${LCDIR}/lib/python ../wiki_example/example_decoder.py encoded_data_p LabComm2014.v1

	@echo "************ running C# decoder: *****************"
	@mono ./ExampleDecoder.exe encoded_data_p

	@echo "************ running C# encoder: *****************"
	@mono ./ExampleEncoder.exe encoded_data_cs

	@echo "************ running Java  decoder: *****************"
	@java -cp .:${LCDIR}/lib/java/labcomm2014.jar:${GENDIR} Decoder encoded_data_cs

	@echo "************ running C decoder: *****************"
	@LD_LIBRARY_PATH=${LCDIR}/lib/c/ ./example_decoder encoded_data_cs

	@echo "************ running python decoder (from wiki_example):"
	@PYTHONPATH=${LCDIR}/lib/python ../wiki_example/example_decoder.py encoded_data_cs LabComm2014.v1

	@echo "************ running C# decoder: *****************"
	@mono ./ExampleDecoder.exe encoded_data_cs

clean:
	rm -rf ${GENDIR}

distclean: clean
	rm -f ${EXECUTABLES}
	rm -f encoded_data_c
	rm -f encoded_data_cs
	rm -f encoded_data_j
	rm -f encoded_data_p
	rm -f labcomm.dll

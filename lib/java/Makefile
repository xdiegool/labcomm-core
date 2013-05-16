MODULES=LabCommDispatcher \
	LabCommDecoderRegistry \
	LabComm \
	LabCommSample \
	LabCommHandler \
	LabCommEncoderRegistry \
	LabCommDecoder \
	LabCommType \
	LabCommEncoderChannel \
	LabCommEncoder \
	LabCommDecoderChannel \
	LabCommWriter \
	LabCommReader \
	WriterWrapper

all: labcomm.jar

labcomm.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm/*.class

gen:
	mkdir gen

gen/JAVAC: $(MODULES:%=se/lth/control/labcomm/%.java) Makefile | gen
	javac -d gen $(filter %.java, $^)
	touch $@

clean:
	rm -rf labcomm.jar gen

MODULES=LabCommDispatcher \
	LabCommDecoderRegistry \
	LabComm \
	LabCommSample \
	LabCommHandler \
	LabCommEncoderRegistry \
	LabCommType \
	LabCommEncoder \
	LabCommEncoderChannel \
	LabCommDecoder \
	LabCommDecoderChannel \
	LabCommWriter \
	LabCommReader \
	WriterWrapper

all: labcomm.jar labcomm20141009.jar labcomm2006.jar

labcomm.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm/*.class se/lth/control/labcomm2006/*.class

labcomm20141009.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm/*.class

labcomm2006.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm2006/*.class

gen:
	mkdir gen

gen/JAVAC: $(MODULES:%=se/lth/control/labcomm/%.java) $(MODULES:%=se/lth/control/labcomm2006/%.java) Makefile | gen
	javac -d gen $(filter %.java, $^)
	touch $@


.PHONY: clean

clean:
	rm -rf labcomm.jar labcomm2006.jar labcomm20141009.jar gen 


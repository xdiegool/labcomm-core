MODULES=Constant \
	Decoder \
	DecoderChannel \
	DecoderRegistry \
	Encoder \
	EncoderChannel \
	EncoderRegistry \
	Reader \
	Sample \
	SampleDispatcher \
	SampleHandler \
	SampleType \
	BuiltinType \
	TypeDef \
	TypeBinding \
	ASTbuilder \
	TypeDefParser \
	Writer \
	WriterWrapper

all: labcomm.jar labcomm2014.jar labcomm2006.jar

labcomm.jar: gen/JAVAC
	echo $@
	cd gen ; jar cfm ../$@ \
		se/lth/control/labcomm/*.class \
		se/lth/control/labcomm2006/*.class

labcomm2014.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm/*.class

labcomm2006.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm2006/*.class

gen:
	mkdir gen

gen/JAVAC: $(MODULES:%=se/lth/control/labcomm/%.java) \
	   $(MODULES:%=se/lth/control/labcomm2006/%.java) \
	   Makefile | gen
	javac -cp ../../compiler/labcomm_compiler.jar -d gen \
            $(filter %.java, $^)
	touch $@


.PHONY: clean
clean:
	rm -rf gen

.PHONY: distclean
distclean: clean
	rm -rf labcomm.jar labcomm2006.jar labcomm2014.jar

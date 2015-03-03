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

.PHONY: all
all: labcomm2006.jar

labcomm2006.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm2006/*.class

gen:
	mkdir gen

gen/JAVAC: $(MODULES:%=se/lth/control/labcomm2006/%.java) \
	   Makefile | gen
	javac -cp ../../compiler/labcomm2006_compiler.jar -d gen \
            $(filter %.java, $^)
	touch $@


.PHONY: test
test:

.PHONY: clean
clean:
	rm -rf gen

.PHONY: distclean
distclean: clean
	rm -rf labcomm2006.jar 

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
	SigTypeDef \
	SigSampleDef \
	ParsedTypeDef \
	ParsedSampleDef \
	Writer \
	WriterWrapper \
	DataType \
	VoidType \
	SignatureSymbolVisitor \
	SignatureSymbol \
	TypeSymbol \
	SampleSymbol \
	NameSymbol \
	SigPrimitiveType \
	SigStructType \
	SigField \
	SigArrayType \
	SigUserType 




.PHONY: all
all: labcomm2014.jar

labcomm2014.jar: gen/JAVAC
	echo $@
	cd gen ; jar cf ../$@ se/lth/control/labcomm2014/*.class

gen:
	mkdir gen

gen/JAVAC: $(MODULES:%=se/lth/control/labcomm2014/%.java) \
	   Makefile | gen
	javac -cp ../../compiler/labcomm2014_compiler.jar -d gen \
            $(filter %.java, $^)
	touch $@


.PHONY: test
test:

.PHONY: clean
clean:
	rm -rf gen

.PHONY: distclean
distclean: clean
	rm -rf labcomm2014.jar

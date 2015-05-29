MODULES=Constant\
	Decoder \
	DecoderChannel \
	DecoderRegistry \
	Encoder \
	EncoderChannel \
	EncoderRegistry \
	RenamingDecoder \
	RenamingEncoder \
	RenamingRegistry \
	Sample \
	SampleDispatcher \
	SampleHandler \
	SampleType \
	WrappingDecoder \
	WrappingEncoder

.PHONY: all
all: labcomm2014.dll

labcomm2014.dll: $(MODULES:%=se/lth/control/labcomm2014/%.cs) Makefile
	mcs -out:$@ -target:library $(filter %.cs, $^)

.PHONY: test
test:

.PHONY: clean
clean:

.PHONY: distclean
distclean:
	rm -f labcomm2014.dll

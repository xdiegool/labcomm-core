MODULES=AbstractDecoder \
        Constant\
	Decoder \
	DecoderChannel \
	DecoderRegistry \
	Encoder \
	EncoderChannel \
	EncoderRegistry \
	Sample \
	SampleDispatcher \
	SampleHandler \
	SampleType

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

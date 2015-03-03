MODULES=Constant\
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
all: labcomm2006.dll

labcomm2006.dll: $(MODULES:%=se/lth/control/labcomm2006/%.cs) Makefile
	mcs -out:$@ -target:library $(filter %.cs, $^)

.PHONY: test
test:

.PHONY: clean
clean:

.PHONY: distclean
distclean:
	rm -f labcomm2006.dll

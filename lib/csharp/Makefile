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
all: labcomm.dll

labcomm.dll: $(MODULES:%=se/lth/control/labcomm/%.cs) Makefile
	mcs -out:$@ -target:library $(filter %.cs, $^)

.PHONY: test
test:

.PHONY: clean
clean:

.PHONY: distclean
distclean:
	rm -f labcomm.dll

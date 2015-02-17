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

all: labcomm.dll

labcomm.dll: $(MODULES:%=se/lth/control/labcomm/%.cs) Makefile
	mcs -out:$@ -target:library $(filter %.cs, $^)

clean:

distclean:
	rm -f labcomm.dll

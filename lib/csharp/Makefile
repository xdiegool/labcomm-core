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

all: labcomm.dll

labcomm.dll: $(MODULES:%=se/lth/control/labcomm/%.cs) Makefile
	mcs -out:$@ -target:library $(filter %.cs, $^)

clean:
	rm labcomm.dll

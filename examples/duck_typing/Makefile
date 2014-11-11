LABCOMM_JAR=../../compiler/labcomm_compiler.jar
LABCOMM=java -jar $(LABCOMM_JAR) 

all: gen/animal.py

.PHONY: test
test: gen/animal.py
	PYTHONPATH=../../lib/python:gen ./duck_typing.py


gen/.dir:
	mkdir -p $@

gen/%.py: %.lc | gen/.dir
	$(LABCOMM) --python=$@ $<

.PHONY: clean distclean
clean distclean:
	rm -rf gen


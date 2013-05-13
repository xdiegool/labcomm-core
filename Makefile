export LABCOMM_JAR=$(shell pwd)/compiler/labComm.jar
export LABCOMM=java -jar $(LABCOMM_JAR) 

all: compiler
	$(MAKE) -C lib 
	$(MAKE) -C lib/c run-test

.PHONY: compiler
compiler:
	cd compiler ; ant jar


.PHONY: test
test:
	$(MAKE) -C test -e

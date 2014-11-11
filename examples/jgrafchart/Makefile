LCDIR=../..
LABCOMM_JAR=../../compiler/labcomm_compiler.jar
LABCOMM=java -jar $(LABCOMM_JAR) 
CLASSPATH=.:${LCDIR}/lib/java/labcomm.jar
JAVA_PKG=labcommTCPtest
SAMPLENAME=foo
LCLIBDIR=${LCDIR}/lib/c

LCFILE=jg
AUX=enc.c dec.c
TLCFILE=turtle1
TAUX=turtle_enc.c turtle_dec.c

${JAVA_PKG}/gen/foo.java: ${LCFILE}.lc
	${LABCOMM} --javapackage=${JAVA_PKG}.gen --java=${JAVA_PKG}/gen $<

${JAVA_PKG}/gen/${SAMPLENAME}.class: ${JAVA_PKG}/gen/${SAMPLENAME}.java
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/server/TestServer.class: ${JAVA_PKG}/server/TestServer.java ${JAVA_PKG}/gen/${SAMPLENAME}.class
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/client/TestClient.class: ${JAVA_PKG}/client/TestClient.java ${JAVA_PKG}/gen/${SAMPLENAME}.class
	javac -cp ${CLASSPATH} $<


.PHONY: runjavaserver
runjavaserver : ${JAVA_PKG}/server/TestServer.class
	java -cp ${CLASSPATH} $(<:.class=)

.PHONY: runjavaclient 
runjavaclient : ${JAVA_PKG}/client/TestClient.class
	java -cp ${CLASSPATH} $(<:.class=)


client: client.c ${LCFILE}.c ${AUX} ${AUX:.c=.h}
	${CC} -o $@ client.c ${AUX} ${LCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm

testserver: testserver.c ${LCFILE}.c ${AUX} ${AUX:.c=.h}
	${CC} -o $@ testserver.c ${AUX} ${LCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm

turtleclient: turtleclient.c ${TLCFILE}.c ${TAUX} ${TAUX:.c=.h}
	${CC} -o $@ turtleclient.c ${TAUX} ${TLCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm
${LCFILE}.c : ${LCFILE}.lc
	${LABCOMM} -C ${LCFILE}.lc

${TLCFILE}.c : ${TLCFILE}.lc
	${LABCOMM} -C ${TLCFILE}.lc

all: client testserver ${JAVA_PKG}/server/TestServer.class ${JAVA_PKG}/client/TestClient.class turtleclient

.PHONY: clean
clean :
	rm -f ${LCFILE}.c ${LCFILE}.h client testserver turtleclient
	rm -f ${JAVA_PKG}/server/*.class ${JAVA_PKG}/client/*.class 
	rm -f ${JAVA_PKG}/gen/* 

.PHONY: distclean
distclean: clean 

.PHONY: runclient
runclient : client
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< localhost 9999

.PHONY: runserver
runserver : testserver
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< 9999

.PHONY: runtclient
runtclient : turtleclient
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< localhost 8082

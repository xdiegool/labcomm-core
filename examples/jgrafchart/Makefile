LCDIR=../..
LCC=java -jar ${LCDIR}/compiler/labComm.jar
CLASSPATH=.:${LCDIR}/lib/java/labcomm.jar
JAVA_PKG=labcommTCPtest
SAMPLENAME=foo
LCC=java -jar ${LCDIR}/compiler/labComm.jar
LCLIBDIR=${LCDIR}/lib/c

LCFILE=jg
AUX=enc.c dec.c

${JAVA_PKG}/gen/foo.java: ${LCFILE}.lc
	${LCC} --javapackage=${JAVA_PKG}.gen --java=${JAVA_PKG}/gen $<

${JAVA_PKG}/gen/${SAMPLENAME}.class: ${JAVA_PKG}/gen/${SAMPLENAME}.java
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/server/TestServer.class: ${JAVA_PKG}/server/TestServer.java ${JAVA_PKG}/gen/${SAMPLENAME}.class
	javac -cp ${CLASSPATH} $<

${JAVA_PKG}/client/TestClient.class: ${JAVA_PKG}/client/TestClient.java ${JAVA_PKG}/gen/${SAMPLENAME}.class
	javac -cp ${CLASSPATH} $<


runjavaserver : ${JAVA_PKG}/server/TestServer.class
	java -cp ${CLASSPATH} $(<:.class=)

runjavaclient : ${JAVA_PKG}/client/TestClient.class
	java -cp ${CLASSPATH} $(<:.class=)


client: client.c ${LCFILE}.c ${AUX} ${AUX:.c=.h}
	${CC} -o $@ client.c ${AUX} ${LCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm

testserver: testserver.c ${LCFILE}.c ${AUX} ${AUX:.c=.h}
	${CC} -o $@ testserver.c ${AUX} ${LCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm
	
${LCFILE}.c : ${LCFILE}.lc
	${LCC} -C ${LCFILE}.lc

all: client testserver ${JAVA_PKG}/server/TestServer.class ${JAVA_PKG}/client/TestClient.class

.PHONY: clean runclient runserver runjavaserver runjavaclient
clean :
	rm ${LCFILE}.c ${LCFILE}.h client testserver
	rm  ${JAVA_PKG}/server/*.class ${JAVA_PKG}/client/*.class ${JAVA_PKG}/gen/* 

runclient : client
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< localhost 9999

runserver : testserver
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< 9999

LCDIR=../..
LCC=java -jar ${LCDIR}/compiler/labComm.jar
CLASSPATH=.:${LCDIR}/lib/java/labcomm.jar
JAVA_PKG=labcommTCPtest
SAMPLENAME=foo
LCC=java -jar ${LCDIR}/compiler/labComm.jar
LCLIBDIR=${LCDIR}/lib/c

LCFILE=jg
AUX=enc.c dec.c
TLCFILE=turtle1
TAUX=turtle_enc.c turtle_dec.c

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
	
turtleclient: turtleclient.c ${TLCFILE}.c ${TAUX} ${TAUX:.c=.h}
	${CC} -o $@ turtleclient.c ${TAUX} ${TLCFILE}.c -I${LCLIBDIR} -L${LCLIBDIR} -llabcomm
${LCFILE}.c : ${LCFILE}.lc
	${LCC} -C ${LCFILE}.lc

${TLCFILE}.c : ${TLCFILE}.lc
	${LCC} -C ${TLCFILE}.lc

all: client testserver ${JAVA_PKG}/server/TestServer.class ${JAVA_PKG}/client/TestClient.class turtleclient

.PHONY: clean runclient runserver runjavaserver runjavaclient turtleclient
clean :
	rm ${LCFILE}.c ${LCFILE}.h client testserver turtleclient
	rm  ${JAVA_PKG}/server/*.class ${JAVA_PKG}/client/*.class ${JAVA_PKG}/gen/* 

runclient : client
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< localhost 9999

runserver : testserver
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< 9999

runtclient : turtleclient
	LD_LIBRARY_PATH=${LCLIBDIR} ./$< localhost 8082
